#include <QApplication>
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QTimer>


// ==========================================================
// CLASSE BACKEND: Gestisce Arduino, ChatGPT e traduce i dati
// ==========================================================
class SynthBackend : public QObject {
    Q_OBJECT

public:
    SynthBackend(QObject *parent = nullptr) : QObject(parent) {
        // 1. Inizializza i manager
        m_serial = new QSerialPort(this);
        m_networkManager = new QNetworkAccessManager(this);

        // 2. Connetti i segnali agli slot (le funzioni che rispondono)
        // Quando Arduino ha dati pronti, leggi
        connect(m_serial, &QSerialPort::readyRead, this, &SynthBackend::readFromArduino);
        // Quando ChatGPT risponde, gestisci la risposta
        connect(m_networkManager, &QNetworkAccessManager::finished, this, &SynthBackend::handleChatGPTResponse);

        // 3. Avvia la seriale
        setupSerial();
    }

    // --- FUNZIONE 2: Invia Richiesta API a Gemini (FASE: SOFTWARE -> CLOUD) ---
    void sendRequestToChatGPT(const QString &keywords) { // Mantengo lo stesso nome per non farti cambiare tutto
        qDebug() << "... Sto chiedendo a Google Gemini ...";

        // !!! INSERISCI QUI LA TUA API KEY DI GEMINI !!!
        QString apiKey = "AIzaSyDeL4nNz3pBPstb6-UxDncsLcYmcSBeYXo";

        // URL di Gemini 1.5 Flash (veloce e gratuito)
        // Usiamo la versione v1 (più stabile) e il modello gemini-1.5-flash aggiornato
QUrl url(QString("https://generativelanguage.googleapis.com/v1/models/gemini-2.5-flash:generateContent?key=%1").arg(apiKey));
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        // Costruzione del prompt
        QString istruzioni = "Sei un compositore per Arduino. Rispondi SOLO con dati grezzi nel formato stringa: "
                             "'F:frequenza,D:durata;F:frequenza,D:durata;...;E'. "
                             "Non scrivere testo o spiegazioni. Genera 8 note casuali basate su queste emozioni: ";

        // Struttura JSON per Gemini
        QJsonObject textObj;
        textObj["text"] = istruzioni + keywords;

        QJsonObject partObj;
        partObj["parts"] = QJsonArray{textObj};

        QJsonObject contentObj;
        contentObj["contents"] = QJsonArray{partObj};

        m_networkManager->post(request, QJsonDocument(contentObj).toJson());
    }

private slots:
    // --- SLOT A: Legge da Arduino (FASE: INPUT FIZICO -> SOFTWARE) ---
    void readFromArduino() {
        // Accumuliamo i dati finché non arriva '\n' (fine prompt)
        m_serialBuffer.append(m_serial->readAll());

        if (m_serialBuffer.contains('\n')) {
            QString promptRaw = QString::fromUtf8(m_serialBuffer).trimmed();
            m_serialBuffer.clear(); // Pulisci buffer per la prossima volta

            // Aspettiamo un formato tipo: "GENERA:triste,lento,piano"
            if (promptRaw.startsWith("GENERA:")) {
                QString keywords = promptRaw.mid(7); // Prendi tutto dopo "GENERA:"
                qDebug() << ">>> Arduino chiede melodia per:" << keywords;
                sendRequestToChatGPT(keywords);
            }
        }
    }

    // --- SLOT B: Gestisce Risposta Gemini (FASE: CLOUD -> SOFTWARE) ---
    void handleChatGPTResponse(QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            QJsonObject root = doc.object();

            // Navighiamo nel JSON di Gemini: candidates[0] -> content -> parts[0] -> text
            QJsonArray candidates = root["candidates"].toArray();
            if (!candidates.isEmpty()) {
                QJsonObject firstCandidate = candidates.at(0).toObject();
                QJsonObject content = firstCandidate["content"].toObject();
                QJsonArray parts = content["parts"].toArray();
                QString assistantReply = parts.at(0).toObject()["text"].toString().trimmed();

                qDebug() << "--- Melodia ricevuta da Gemini ---";
                qDebug() << assistantReply;

                sendMelodyToArduino(assistantReply);
            }
        } else {
            qDebug() << "!!! Errore API Gemini:" << reply->errorString();
            qDebug() << "Risposta server:" << reply->readAll();
        }
        reply->deleteLater();
    }

private:
    // --- FUNZIONE 1: Setup Seriale ---
    void setupSerial() {
        // PRO MEMORIA: Assicurati che Arduino sia collegato!
        QString arduinoPortName = "";
        const auto infos = QSerialPortInfo::availablePorts();
        for (const QSerialPortInfo &info : infos) {
            // Cerchiamo una porta che sembri Arduino (spesso ha "Arduino" o un driver specifico)
            qDebug() << "Porta trovata:" << info.portName() << info.description();
            if (info.description().contains("Arduino", Qt::CaseInsensitive)) {
                arduinoPortName = info.portName();
                break;
            }
        }

        if (arduinoPortName.isEmpty()) {
            qDebug() << "!!! Arduino non trovato. Verifica collegamento.";
            return;
        }

        m_serial->setPortName(arduinoPortName);
        m_serial->setBaudRate(QSerialPort::Baud9600); // Deve matchare con Arduino: Serial.begin(9600)
        m_serial->setDataBits(QSerialPort::Data8);
        m_serial->setParity(QSerialPort::NoParity);
        m_serial->setStopBits(QSerialPort::OneStop);
        m_serial->setFlowControl(QSerialPort::NoFlowControl);

        if (m_serial->open(QIODevice::ReadWrite)) {
            qDebug() << ">>> Connesso ad Arduino su" << arduinoPortName;
            // TEST TEMPORANEO: Simulo che Arduino abbia appena inviato una richiesta dopo 3 secondi
            QTimer::singleShot(3000, this, [this](){
                qDebug() << "[TEST] Simulo input Arduino...";
                this->m_serialBuffer = "GENERA:triste,lento,piano\n";
                this->readFromArduino(); // Chiamo forzatamente lo slot di lettura
            });
        } else {
            qDebug() << "!!! Impossibile aprire porta seriale:" << m_serial->errorString();
        }
    }



    // --- FUNZIONE 3: Invia Melodia (FASE: SOFTWARE -> ARDUINO) ---
    void sendMelodyToArduino(const QString &melodyData) {
        if (m_serial->isOpen()) {
            qDebug() << ">>> Invio dati musicali ad Arduino:" << melodyData;
            // Aggiungiamo '\n' così Arduino capisce che la stringa è finita
            m_serial->write(melodyData.toUtf8() + '\n');
        } else {
            qDebug() << "!!! Seriale non aperta, impossibile inviare melodia.";
        }
    }

    // Memoria della classe
    QSerialPort *m_serial;
    QByteArray m_serialBuffer;
    QNetworkAccessManager *m_networkManager;
};

// ==========================================================
// MAIN: Punto di ingresso dell'applicazione
// ==========================================================
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // Creiamo il "cervello" del backend
    SynthBackend backend;

    qDebug() << "===========================================";
    qDebug() << "Aether-Synth Backend Attivo.";
    qDebug() << "In attesa di prompt da Arduino (formato: GENERA:...)";
    qDebug() << "===========================================";

    // Dopo aver creato l'oggetto backend:
    backend.sendRequestToChatGPT("Jazz, Allegro, Tromba");

    // MainWindow di default (vuota per ora, serve al tuo compagno GUI)
    // MainWindow w;
    // w.show();

    return a.exec(); // Avvia il ciclo degli eventi (necessario per seriale e network)
}

// Necessario per Qt quando si definiscono classi con segnali/slot nel main.cpp
#include "main.moc"