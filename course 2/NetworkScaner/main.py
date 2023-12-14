
import sys

from PyQt5.QtWidgets import QApplication

from app import App


def main():
    app = QApplication(sys.argv)
    _ = App()
    exit(app.exec())


if __name__ == '__main__':
    main()
