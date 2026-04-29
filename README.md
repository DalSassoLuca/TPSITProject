# Aether-Synth AI (v2.0) 🎹🤖

> Un controller musicale intelligente che fonde l'AI generativa (Google Gemini) con il controllo gestuale hardware.

## 📖 Descrizione del Progetto
**Aether-Synth AI** è un sistema integrato dove l'interazione fisica incontra l'intelligenza artificiale. Attraverso un'interfaccia Arduino, l'utente seleziona "mood" o parametri emozionali che vengono inviati a un'applicazione PC (Qt Creator). Quest'ultima interroga l'IA **Google Gemini** per generare una melodia originale, che viene poi eseguita dall'hardware. 

Il punto di forza della versione 2.0 è l'integrazione di un **sensore a ultrasuoni**, che permette all'utente di modulare il suono (volume o pitch) muovendo la mano, similmente a un Theremin.

---

## 🛠️ Hardware & Software

| Categoria | Elemento | Descrizione |
| :--- | :--- | :--- |
| **Hardware** | Arduino Uno/Nano | Microcontrollore principale |
| **Display** | OLED I2C | Visualizzazione menu e prompt |
| **Sensore** | HC-SR04 | Controllo gestuale (distanza mano) |
| **Audio** | Buzzer Passivo | Generazione del suono |
| **AI Engine** | Google Gemini API | Generazione della logica musicale |
| **App Desktop** | Qt Creator (C++) | Bridge seriale e gestione API |

---

## 👥 Il Team (Suddivisione Ruoli)

### 🔧 Membro 1: Firmware & Electronics (Bragagnolo)
* **Gestione Sensori:** Implementazione dell'HC-SR04.
* **Logica Theremin:** Mappatura distanza/frequenza in real-time.
* **UI Hardware:** Gestione del menu OLED tramite pulsanti.

### 💻 Membro 2: Software Architect & AI (Dal Sasso)
* **Bridge Seriale:** Comunicazione stabile via `QSerialPort`.
* **Cloud Logic:** Integrazione API Google Gemini v1.
* **Data Parsing:** Formattazione stringhe musicali (es. `F:xxx,D:yyy`).

### 🎨 Membro 3: UI & CAD Design (Misciagna)
* **GUI Desktop:** Interfaccia di monitoraggio e visualizzazione partitura.
* **Design 3D:** Progettazione e stampa del case del controller.
* **Assemblaggio:** Rifinitura estetica e montaggio componenti.

---

## 🚀 Roadmap di Sviluppo
- [ ] **Fase 1:** Test comunicazione seriale bidirezionale PC <-> Arduino.
- [ ] **Fase 2:** Ottimizzazione prompt Gemini per output in formato dati puro.
- [ ] **Fase 3:** Implementazione controllo dinamico su Arduino.
- [ ] **Fase 4:** Assemblaggio finale nel case personalizzato.

---

## 📡 Protocollo di Comunicazione
I dati vengono scambiati tra Qt e Arduino utilizzando un protocollo a stringhe:
`F:[Frequenza],D:[Durata];` 
*Esempio: `F:440,D:500;` suona un La per 500ms.*
