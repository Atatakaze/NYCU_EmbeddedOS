import sys
from PyQt5.QtWidgets import QApplication, QWidget, QLabel, QPushButton
from PyQt5.QtGui import QIcon, QPalette, QBrush, QPixmap, QFont
from PyQt5.QtCore import pyqtSlot

mode = 1

class MainWidget(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle("Taiko Drum Master")
        self.resize(650, 400)
   
        # background
        palette = QPalette()
        pix = QPixmap("./images/startpage.png")
        pix = pix.scaled(self.width(), self.height())
        palette.setBrush(QPalette.Background, QBrush(pix))
        self.setPalette(palette)

        # label text
        self.welcomeLabel = QLabel("Which mode to play", self)
        self.welcomeLabel.move(190, 180)
        self.welcomeLabel.setFont(QFont('Arial', 20))

        # button 1 & 2
        self.button1 = QPushButton('Stand-alone', self)
        self.button1.setGeometry(190, 280, 130, 60)
        self.button1.setStyleSheet('font-size:20px;')
        self.button1.clicked.connect(self.onButtonClick)

        self.button2 = QPushButton('Dual Mode', self)
        self.button2.setGeometry(350, 280, 130, 60)
        self.button2.setStyleSheet('font-size:20px;')
        self.button2.clicked.connect(self.onButtonClick)

    def onButtonClick(self):
        sender = self.sender()

        if sender == self.button1:
            print("Select stand-alone mode.")
            self.welcomeLabel.setText("Stand-alone mode.")
            #self.hide()
            mode = 1
        if sender == self.button2:
            print("Select dual mode.")
            self.welcomeLabel.setText("Dual mode.")
            mode = 2

if __name__ == '__main__':
    app = QApplication(sys.argv)

    mainPage = MainWidget()
    mainPage.show()

    sys.exit(app.exec_())
