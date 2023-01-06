import time
import sys
import socket
from threading import Timer
import numpy as np

from PyQt5.QtWidgets import QApplication, QWidget, QLabel, QPushButton
from PyQt5.QtGui import QPixmap, QFont
from PyQt5.QtCore import pyqtSlot, QThread, pyqtSignal, QCoreApplication, Qt

class Beat:
	def __init__(self,hit_type=1,section=0,pos=0,time=0.0,end_time=0.0,scroll_speed=1.0,offset=-1):
		self.type=hit_type
		self.section=section
		self.pos=pos
		self.time=time
		self.end_time=end_time
		self.scroll_speed=scroll_speed
		self.offset=offset
	def show(self):
		print("type : ",self.type," time : ",self.time," scroll_speed : ",self.scroll_speed)

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
        self.beats=[]           #all beats (2000)
        #self.current_beats=[]   #show beats 
        self.initUI()
        self.startPoint=0
        self.endPoint=0
        self.pause=False

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
        self.welcomeLabel = QLabel("Press start button !!!", self)
        self.welcomeLabel.setFont(QFont('Arial', 20))
        self.welcomeLabel.setAlignment(Qt.AlignCenter)

        # button 1 & 2
        self.button1 = QPushButton('START', self)
        self.button1.setStyleSheet('font-size:20px;')
        self.button1.clicked.connect(self.onButtonClick)

        # ========================================== #
        #          Game page (stand-alone)           #
        # ========================================== #

        # background
        self.game_background_img = QLabel(self)
        pixmap = QPixmap("./images/gamepage.png")
        self.game_background_img.setPixmap(pixmap)
        self.game_background_img.setScaledContents(True)


        # ========================================== #
        #                    Beats                   #
        # ========================================== #

        # beats
        self.redBeats = []
        self.blueBeats = []
        self.BeatsCal()
        self.beats = self.beats[0:300]
        pixmap_r = QPixmap("./images/red.png")
        pixmap_b = QPixmap("./images/blue.png")
        
        for i in range(30):
            r_img = QLabel(self)
            r_img.setPixmap(pixmap_r)
            r_img.setScaledContents(True)
            self.redBeats.append(r_img)
            b_img = QLabel(self)
            b_img.setPixmap(pixmap_b)
            b_img.setScaledContents(True)
            self.blueBeats.append(b_img)
        
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
        self.image_init()
        self.repaint()
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
        self.timeinterval = 0.05
        self.timer = RepeatTimer(self.timeinterval, self.timerInterrupt)
        self.timer.start()

    def timerInterrupt(self):
        if not self.pause:
            self.counter += 1
        if self.page == 'stand-alone':
            self.update_screen()
            
    def shut_down(self):
        self.sockClient.stop()
        self.sockfd.close()
        self.timer.cancel()
        QCoreApplication.instance().quit()

    def threadUpdate(self, receive_buf):
        if self.page == 'main page':
            if receive_buf == 'Confirm Select single mode':
                self.page = 'stand-alone'
                self.counter=0
                self.score=0
                self.startPoint=0
                self.endPoint=0
                self.pause=False
                self.repaint()
            
        if receive_buf == 'shut down':
            self.shut_down()
        if self.page == 'stand-alone':
            if receive_buf == 'end':
                self.page = 'main page'

                self.repaint()
            if receive_buf == 'pause':
                if self.pause:
                    self.pause=False
                else:
                    self.pause=True
            if receive_buf == '1':
                score = self.judge(1,self.counter*self.timeinterval)
                if score == 1:
                    self.score += 100            
            if receive_buf == '2':
                score = self.judge(2,self.counter*self.timeinterval)
                if score == 1:
                    self.score += 100

        self.scoreLabel.setText("Score: " + str(self.score))

    def threadFinished(self):
        print(" > [INFO] Thread finished.")

    def onButtonClick(self):
        sender = self.sender()

        if sender == self.button1:
            print(" > [INFO] Select stand-alone mode.")
            self.page = 'stand-alone'
            self.repaint()

        if sender == self.return_button:
            print(" > [INFO] Return to main page.")
            self.page = 'main page'
            self.repaint()

    def image_init(self):
        self.shut_button.setGeometry(530, 20, 1, 1)        
        self.background_img.setGeometry(0, 0, 650, 400)
        self.welcomeLabel.move(190, 180)
        self.button1.setGeometry(260, 280, 130, 60)
        self.timerLabel.setGeometry(280, 20, 80, 20)
        self.scoreLabel.setGeometry(500, 50, 150, 40)
        self.return_button.setGeometry(530, 50, 1, 1)
        self.game_background_img.setGeometry(0, 0, 650, 400)        
        
    def repaint(self):
        for n in self.redBeats:
            n.setVisible(False)
        for n in self.blueBeats:
            n.setVisible(False)
        
        if self.page == 'main page':
            self.game_background_img.setVisible(False)
            self.timerLabel.setVisible(False)
            self.return_button.setVisible(False)
            self.scoreLabel.setVisible(False)
            self.background_img.setVisible(True)
            self.shut_button.setVisible(True)
            self.welcomeLabel.setVisible(True)
            self.button1.setVisible(True)
        
        if self.page == 'stand-alone':
            self.background_img.setVisible(False)
            self.welcomeLabel.setVisible(False)
            self.button1.setVisible(False)
            self.game_background_img.setVisible(True)
            self.shut_button.setVisible(True)
            self.return_button.setVisible(True)
            self.scoreLabel.setVisible(True)
            self.timerLabel.setVisible(True)
            
    def update_screen(self):
        pos = self.calPosition(self.counter*self.timeinterval)
        if len(pos)==0 and self.counter*self.timeinterval>=1500:
            self.page='main page'
            self.repaint()
        for n in self.redBeats:
            n.setVisible(False)
        for n in self.blueBeats:
            n.setVisible(False)
        r_cnt=0
        b_cnt=0
        try:
            for i in range(self.startPoint,self.endPoint):
                if self.beats[i].type == 1 or self.beats[i].type == 3:
                    self.redBeats[r_cnt].move(pos[i-self.startPoint],135)
                    self.redBeats[r_cnt].setVisible(True)
                    r_cnt+=1               
                else:
                    self.blueBeats[b_cnt].move(pos[i-self.startPoint],135)
                    self.blueBeats[b_cnt].setVisible(True)
                    b_cnt+=1
        except:
            print("00")
    
    def BeatsCal(self):
        offset = -5
        f = open("test.txt","r+")
        temp = f.read()
        arr1 = temp.split('\n')
        arr=[]
        for i in range(len(arr1)):
            if i % 4 ==0:
                arr.append(arr1[i])
        for i in arr:
            a = []
            cla = i.split(',')
            for j in cla:
                cla2 = j.split(':')
                a.append(float(cla2[1]))
            if a[3] > 30:
                break		    
            self.beats.append(Beat(a[0],a[1],a[2],(a[3]-offset),a[4],a[5]))
        return
	    
    def judge(self,hit_button,hit_time):
	    if(self.startPoint == len(self.beats)+1):
		    return 0
	    jud = (float)((float)(self.beats[self.startPoint].time) - (float)(hit_time))
	    jud = jud * 1000
	    jud = round(jud,2)
	    if jud > 200:
		    return 0	    
	    # red button
	    if hit_button == 1:
		    # if blue, return
		    while jud < -200:
			    self.startPoint = self.startPoint + 1
			    jud = self.beats[self.startPoint].time - hit_time
			    jud = jud * 1000
			    if jud > 200 :
				    return 0		    
		    jud=abs(jud)	
		    if self.beats[self.startPoint].type == 2 or self.beats[self.startPoint].type == 4:
		        return 0	    
		    if jud < 200 and jud > 100:
			    self.startPoint = self.startPoint + 1
			    return 	2		    
		    if jud <100:
			    self.startPoint = self.startPoint + 1
			    return 	1
	
	    # blue button
	    if hit_button == 2:
		    # if blue, return
		    while jud < -200:
			    self.startPoint = self.startPoint + 1
			    jud = self.beats[self.startPoint].time - hit_time
			    jud = jud * 1000
			    if jud > 200 :
				    return 0		    
		    jud=abs(jud)		
		    if self.beats[self.startPoint].type == 1 or self.beats[self.startPoint].type == 3:
		        return 0    
		    if jud < 200 and jud > 100:
			    self.startPoint = self.startPoint + 1
			    return 	2	    
		    if jud <100:
			    self.startPoint = self.startPoint + 1
			    return 	1
	
    def calPosition(self,time,posMin=102,posMax=650,judgePoint=148):
	    pos = []	    
	    if self.startPoint >= len(self.beats):
		    return pos

	    way_time = 3.5 # 單位：秒，預設1.7秒
	    width = posMax - judgePoint
	    for i in range(self.startPoint,len(self.beats)):
		    x_pos = (self.beats[i].time - time) * (width / way_time) * self.beats[i].scroll_speed + judgePoint;
		    x_pos = round(x_pos,3)
		    
		    if x_pos < posMin:
			    self.startPoint = self.startPoint + 1
			    if i == len(self.beats) - 1:
				    self.startPoint = len(self.beats) + 1
				    self.endPoint = self.startPoint
				    break
			    continue
		    elif x_pos > posMax:
			    self.endPoint = i
			    break
		    elif x_pos < posMax and i == len(self.beats) - 1:
			    self.endpoint = i
			    break
		    pos.append(int(x_pos))
	    return pos
	    
if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("[Usage]: python final_UI.py [port].\n")
        exit()
    port = int(sys.argv[1])
    app = QApplication(sys.argv)
    mainPage = MainWidget(port)
    mainPage.show()
    sys.exit(app.exec_())
