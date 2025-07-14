
# PSLabel – The Future of Smart eInk Labeling

> **Low-power | Wireless | Modular | Open Source**

<p align="center">
  <a href="https://github.com/hprs-in/PSLabel/actions/workflows/build-using-docker.yml?query=branch%3Amain">
    <img src="https://github.com/hprs-in/PSLabel/actions/workflows/build-using-docker.yml/badge.svg?event=push" alt="Build Status">
  </a>
  <a href="https://github.com/hprs-in/PSLabel/actions/workflows/docs.yml?query=branch%3Amain">
    <img src="https://github.com/hprs-in/PSLabel/actions/workflows/docs.yml/badge.svg?event=push" alt="Docs Build">
  </a>
  <a href="https://hprs-in.github.io/PSLabel/">
    <img src="https://img.shields.io/badge/documentation-3D578C?logo=sphinx&logoColor=white" alt="Documentation">
  </a>
  <a href="https://hprs-in.github.io/PSLabel/doxygen">
    <img src="https://img.shields.io/badge/API-documentation-3D578C?logo=c&logoColor=white" alt="API Documentation">
  </a>
    <a href="https://sonarcloud.io/summary/new_code?id=hprs-in_PSLabel">
    <img src="https://sonarcloud.io/api/project_badges/measure?project=hprs-in_PSLabel&metric=alert_status" alt="Quality Gate Status">
  </a>
</p>

[![SonarQube Cloud](https://sonarcloud.io/images/project_badges/sonarcloud-light.svg)](https://sonarcloud.io/summary/new_code?id=hprs-in_PSLabel)

**PSLabel** is an open-source platform for creating **scalable, ultra-low-power eInk label systems**. Designed for retail, logistics, healthcare, smart offices, and smart homes — PSLabel combines **BLE + NFC**, **modular firmware**, and **cloud-ready OTA** capabilities to redefine digital labeling.

---

## 🌟 Vision

To build the **de facto standard for smart, wireless labeling** in embedded environments using open technologies — empowering developers and businesses to integrate **dynamic, energy-efficient visual displays** in real-world workflows.

---

## 🔧 Tech Stack

- **Firmware**: Zephyr RTOS / nRF Connect SDK (Nordic)
- **MCU**: nRF52 and nRF54 Series (Build available for nRF54L15DK, nRF52840DK)
- **Communication**: BLE Peripheral + NFC Wake
- **Display Driver**: eInk (GoodDisplay/Waveshare) over SPI
- **Power**: Coin cell – optimized sleep/wake cycle
- **OTA Support**: BLE
- **App Layer**: Android/iOS/Web BLE Gateway (future)

---

## 🚀 Use Cases

| Domain         | Use Case                          |
|----------------|-----------------------------------|
| Warehousing    | Shelf labels, bin IDs, scan-to-update |
| Retail         | Wireless price tags, expiry tracking |
| Healthcare     | Dynamic patient labels, dosage logs |
| Smart Office   | Door signs, desk status, meeting labels |
| Smart Home     | Task boards, inventory tags, reminders |

---

## 📦 Project Structure

```
pslabel/
├── app/                # Zephyr app: main, config, drivers
├── boards/             # Custom board overlays
├── include/            # Shared headers
├── tools/              # Python tools for BLE image upload
└── docs/               # Diagrams, documentation
```

---

## 🔄 System Architecture

```
[ Smartphone NFC ]
        |
[NFC Wake Trigger] --> [PSLabel Device (BLE On)]
        |
[Mobile App/Web BLE]
        |
[BLE GATT Transfer Image]
        |
[Render eInk via SPI]
        |
[Enter Deep Sleep]
```

---

## 📅 Roadmap (2025 – 2026)

- ✅ NFC wake-up + BLE bootstrapping
- ✅ BLE GATT image transfer
- ✅ SPI-based eInk display rendering
- 🔜 BLE OTA using MCUBoot
- 🔜 Mobile App (Flutter) for BLE upload
- 🔜 NFC provisioning (BLE keys, config)
- 🔜 Multi-label sync via gateway
- 🔜 Demo

---

## 🤝 Contributing

We welcome contributions from embedded engineers, UX designers, and technical writers. Start here:

- Fork the repo
- Check open [Issues](https://github.com/hprs-in/PSLabel/issues)
- Add features, optimize power, or improve image handling

See [CONTRIBUTING.md](CONTRIBUTING.md) for coding style and guidelines.

---

## 📚 References & Inspirations

- [Nordic Semiconductor nRF Connect SDK Examples](https://github.com/nrfconnect/ncs-example-application)
- [Zephyr RTOS](https://zephyrproject.org/)
- [GoodDisplay / Waveshare](https://www.waveshare.com/)

---

## 📜 License

This project is licensed under the MIT License – see [LICENSE](LICENSE) for details.

---

## 👨‍💻 Maintainers & Community

Developed and maintained by **[HPRS.in](https://hprs.in)**  
