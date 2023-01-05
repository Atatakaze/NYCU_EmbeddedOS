import time
import sys
import socket
from threading import Timer
import numpy as np

from PyQt5.QtWidgets import QApplication, QWidget, QLabel, QPushButton
from PyQt5.QtGui import QPixmap, QFont
from PyQt5.QtCore import pyqtSlot, QThread, pyqtSignal, QCoreApplication, Qt

class SocketClient(QThread):
    trigger = pyqtSignal(str)
    finished = pyqtSignal()

    def __init__(self, sockfd):
        super().__init__()
        self.sockfd = sockfd
        self.running = True

    def run(self):
        self.connfd, self.addr = self.sockfd.accept()
        print('[Server]: Connected by ', str(self.addr))

        while(self.running):
            receive_buf = self.connfd.recv(1024).decode()
            print('[Server]: Receive ', receive_buf)
            self.trigger.emit(receive_buf)

    def stop(self):
        self.running = False
        self.finished.emit()


class RepeatTimer(Timer):
    def run(self):
        self.finished.wait(self.interval)
        while not self.finished.is_set():
            self.function(*self.args, **self.kwargs)
            self.finished.wait(self.interval)


class MainWidget(QWidget):
    def __init__(self, port):
        super().__init__()
        self.score = 0
        self.port = port
        self.initUI()

    def initUI(self):
        self.setWindowTitle("Taiko Drum Master")
        self.resize(650, 400)
        self.page = 'main page'
   
        # ========================================== #
        #               Main page                    #
        # ========================================== #

        # background
        self.background_img = QLabel(self)
        pixmap = QPixmap("./images/startpage.png")
        self.background_img.setPixmap(pixmap)
        self.background_img.setScaledContents(True)

        # shut down button
        self.shut_button = QPushButton(self)
        self.shut_button.setShortcut('Ctrl+C')
        self.shut_button.clicked.connect(self.shut_down)

        # label text
        self.welcomeLabel = QLabel("Which mode to play ?", self)
        self.welcomeLabel.setFont(QFont('Arial', 20))
        self.welcomeLabel.setAlignment(Qt.AlignCenter)

        # button 1 & 2
        self.button1 = QPushButton('Stand-alone', self)
        self.button1.setStyleSheet('font-size:20px;')
        self.button1.clicked.connect(self.onButtonClick)

        self.button2 = QPushButton('Dual Mode', self)
        self.button2.setStyleSheet('font-size:20px;')
        self.button2.clicked.connect(self.onButtonClick)

        # ========================================== #
        #          Game page (stand-alone)           #
        # ========================================== #

        # background
        self.game_background_img = QLabel(self)
        pixmap = QPixmap("./images/gamepage.png")
        self.game_background_img.setPixmap(pixmap)
        self.game_background_img.setScaledContents(True)

        # beats
        self.redBeats = []
        self.blueBeats = []
        pixmap_r = QPixmap("./images/red.png")
        pixmap_b = QPixmap("./images/blue.png")
        for i in range(20):
            r_img = QLabel(self)
            r_img.setPixmap(pixmap_r)
            r_img.setScaledContents(True)
            self.redBeats.append(r_img)

            b_img = QLabel(self)
            b_img.setPixmap(pixmap_b)
            b_img.setScaledContents(True)
            self.blueBeats.append(b_img)

        # if the beats visible or not red[20], blue[20]
        self.sequence = np.zeros((2, 20))

        # timer label
        self.timerLabel = QLabel(self)
        self.timerLabel.setAlignment(Qt.AlignCenter)

        # score board
        self.scoreLabel = QLabel("Score: 0", self)
        self.scoreLabel.setFont(QFont('Arial', 15, QFont.Bold))
        self.scoreLabel.setAlignment(Qt.AlignCenter)

        # return button
        self.return_button = QPushButton(self)
        self.return_button.setShortcut('Ctrl+D')
        self.return_button.clicked.connect(self.onButtonClick)

        # ========================================== #
        #                 Display                    #
        # ========================================== #
        self.paint()

        # ========================================== #
        #           socket connetion                 #
        # ========================================== #

        self.sockfd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sockfd.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sockfd.bind(('', self.port))
        self.sockfd.listen(2)

        print('[Server]: Start at port %s'.format(str(self.port)))
        print('[Server]: Wait for connection ...')   

        # socket connection
        self.sockClient = SocketClient(self.sockfd)
        self.sockClient.start()
        self.sockClient.trigger.connect(self.threadUpdate)
        self.sockClient.finished.connect(self.threadFinished)

        # ========================================== #
        #                 Timer                      #
        # ========================================== #
        self.counter = 0
        self.timer = RepeatTimer(0.01, self.timerInterrupt)
        self.timer.start()

    def timerInterrupt(self):
        self.counter += 1
        self.timerLabel.setText('Counter ' + str(self.counter / 100))
        self.repaint()

    def shut_down(self):
        self.sockClient.stop()
        self.sockfd.close()
        self.timer.cancel()
        QCoreApplication.instance().quit()

    def threadUpdate(self, receive_buf):
        if receive_buf == 'stand-alone':
            self.page = 'stand-alone'
        
        if receive_buf == 'pause':
            self.page = 'main page'

        if receive_buf == 'shut down':
            self.shut_down()

        if receive_buf == 'red':
            self.score += 1
            self.sequence[0][0] = 1
            self.sequence[1][0] = 0

        if receive_buf == 'blue':
            self.score += 1
            self.sequence[0][0] = 0
            self.sequence[1][0] = 1

        self.scoreLabel.setText("Score: " + str(self.score))
        self.repaint()

    def threadFinished(self):
        print(" > [INFO] Thread finished.")

    def onButtonClick(self):
        sender = self.sender()

        if sender == self.button1:
            print(" > [INFO] Select stand-alone mode.")
            self.page = 'stand-alone'
            self.repaint()
        if sender == self.button2:
            print("> [INFO] Select dual mode.")
            self.page = 'stand-alone'
            self.repaint()

        if sender == self.return_button:
            print(" > [INFO] Return to main page.")
            self.page = 'main page'
            self.repaint()

    def paint(self):
        self.shut_button.setGeometry(530, 20, 1, 1)
        self.shut_button.setVisible(True)
        
        self.background_img.setGeometry(0, 0, 650, 400)
        self.background_img.setVisible(True)

        self.welcomeLabel.move(190, 180)
        self.welcomeLabel.setVisible(True)

        self.button1.setGeometry(190, 280, 130, 60)
        self.button1.setVisible(True)
        self.button2.setGeometry(350, 280, 130, 60)
        self.button2.setVisible(True)

        self.timerLabel.setGeometry(280, 20, 80, 20)
        self.timerLabel.setVisible(False)

        self.scoreLabel.setGeometry(500, 50, 150, 40)
        self.scoreLabel.setVisible(False)

        self.return_button.setGeometry(530, 50, 1, 1)
        self.return_button.setVisible(False)

        self.game_background_img.setGeometry(0, 0, 650, 400)
        self.game_background_img.setVisible(False)

        """
        0: 148 right: 620 left: 102
        """
        for i in range(20):
            self.redBeats[i].setGeometry(148, 135, 30, 30)
            self.redBeats[i].setVisible(False)
            self.blueBeats[i].setGeometry(148, 135, 30, 30)
            self.blueBeats[i].setVisible(False)

    def repaint(self):
        if self.page == 'main page':
            self.game_background_img.setVisible(False)
            self.timerLabel.setVisible(False)
            self.return_button.setVisible(False)
            self.scoreLabel.setVisible(False)
            
            for i in range(20):
                self.redBeats[i].move(148, 135)
                self.redBeats[i].setVisible(False)
                self.blueBeats[i].move(148, 135)
                self.blueBeats[i].setVisible(False)
            
            self.background_img.setVisible(True)

            self.shut_button.setVisible(True)
            self.welcomeLabel.setVisible(True)
            self.button1.setVisible(True)
            self.button2.setVisible(True)
        
        if self.page == 'stand-alone':
            self.background_img.setVisible(False)
            self.welcomeLabel.setVisible(False)
            self.button1.setVisible(False)
            self.button2.setVisible(False)

            self.game_background_img.setVisible(True)

            self.shut_button.setVisible(True)
            self.return_button.setVisible(True)
            self.scoreLabel.setVisible(True)
            self.timerLabel.setVisible(True)

            for i in range(20):
                if self.sequence[0][i] == 0:
                    self.redBeats[i].setVisible(False)
                if self.sequence[0][i] == 1:
                    self.redBeats[i].setVisible(True)
                if self.sequence[1][i] == 0:
                    self.blueBeats[i].setVisible(False)
                if self.sequence[1][i] == 1:
                    self.blueBeats[i].setVisible(True)


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("[Usage]: python final_UI.py [port].\n")
        exit()
    port = int(sys.argv[1]) 

    app = QApplication(sys.argv)

    mainPage = MainWidget(port)
    mainPage.show()
    sys.exit(app.exec_())