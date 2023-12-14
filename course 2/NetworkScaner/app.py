from __future__ import annotations
from PyQt5.QtWidgets import (QDialog, QDoubleSpinBox, QGridLayout, QGroupBox,
                             QLabel, QLineEdit, QPushButton, QWidget)

from network_scanner import HostInfo, scan_host, scan_port


class App(QWidget):
    def __init__(self) -> None:
        super().__init__()

        self.__init_ui()

        self.setWindowTitle('Network Scanner')
        self.show()

    def __init_ui(self) -> None:
        main_layout = QGridLayout(self)

        self.ip = QLineEdit()
        main_layout.addWidget(self.ip, 0, 0)
        main_layout.addWidget(QLabel('Target IP'), 0, 1)

        host_scan = QPushButton('Scan host')
        host_scan.clicked.connect(
            lambda: self.__show_host_info(scan_host(self.ip.text())))
        main_layout.addWidget(host_scan, 1, 0, 1, 2)

        main_layout.addWidget(self.__init_port_scan(), 2, 0, 1, 2)

    def __init_port_scan(self) -> QGroupBox:
        def worker() -> None:
            available_ports: dict[int, str] = {}
            for port in App.__parse_ports(ports.text()):
                if port_name := scan_port(self.ip.text(), port, timeout.value()):
                    available_ports[port] = port_name

            self.__show_port_info(available_ports)

        group = QGroupBox('Ports')
        layout = QGridLayout(group)

        ports = QLineEdit()
        layout.addWidget(ports, 0, 0)
        layout.addWidget(QLabel('Ports'), 0, 1)

        timeout = QDoubleSpinBox()
        timeout.setRange(0, 100)
        timeout.setValue(.1)
        timeout.setSingleStep(.1)
        layout.addWidget(timeout, 1, 0)
        layout.addWidget(QLabel('Timeout, s'), 1, 1)

        port_scan = QPushButton('Scan ports')
        port_scan.clicked.connect(worker)
        layout.addWidget(port_scan, 2, 0, 1, 2)

        return group

    def __show_host_info(self, info: HostInfo) -> None:
        def __save() -> None:
            with open(f'host_info_{self.ip.text()}.txt', 'w') as file:
                file.write(data)

        data = f'Country: {info["country"]}\n' + \
            f'Region: {info["regionName"]}\n' + \
            f'Organization: {info["org"]}\n' + \
            f'Coordinates: {info["lat"]},{info["lon"]}'

        msg = QDialog()
        msg.setWindowTitle('Host info')
        layout = QGridLayout(msg)

        layout.addWidget(QLabel(data), 0, 0)

        save = QPushButton('Save')
        save.clicked.connect(__save)
        layout.addWidget(save, 1, 0)

        msg.exec()

    def __show_port_info(self, available_ports: dict[int, str]) -> None:
        def __save() -> None:
            with open(f'port_info_{self.ip.text()}.txt', 'w') as file:
                file.write(data)

        data = '\n'.join(
            map(lambda port: f'{port}: {available_ports[port]}', available_ports))

        msg = QDialog()
        msg.setWindowTitle('Available ports')
        layout = QGridLayout(msg)

        layout.addWidget(QLabel(data), 0, 0)

        save = QPushButton('Save')
        save.clicked.connect(__save)
        layout.addWidget(save, 1, 0)

        msg.exec()

    @staticmethod
    def __parse_ports(text: str) -> list[int]:
        ports = []

        for entry in text.split(','):
            entry = entry.strip()
            if str.isnumeric(entry):
                ports.append(int(entry))
            elif '-' in entry:
                left, _, right = entry.partition('-')
                ports.extend(range(int(left), int(right) + 1))

        return ports
