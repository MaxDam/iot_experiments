import jointdriver as jd
import detector as dt
import argparse
import time
import random
import sensor as ss

#debug mode
DEBUG = False

#parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-cmd", "--command", default=0, help="command")

args = ap.parse_args()


#il robot e' contento
def happy():
    #si abbassa in avanti e guarda in su
    jd.moveJoint(jd.RIGHT_BACK_ARM, 60)
    jd.moveJoint(jd.LEFT_BACK_ARM, 60)
    jd.moveJoint(jd.RIGHT_FRONT_ARM, 0)
    jd.moveJoint(jd.LEFT_FRONT_ARM, 0)
    jd.moveJoint(jd.HEAD, 10)
    time.sleep(0.4)

    #nuove il dietro come un cane
    for degree in [30, -30, 30, -30, 30, -30, 30, -30, 0]:
        jd.moveJoint(jd.RIGHT_BACK_SHOULDER, -degree)
        jd.moveJoint(jd.LEFT_BACK_SHOULDER, degree)
        time.sleep(0.2)


#standup iniziale
if(args.command == "standup"):
    arm_zero_pos = 50
    jd.zero(arm_zero_pos)
    time.sleep(0.5)

    #effettua leggeri movimenti random
    for _ in range(50):
        mov = random.choice([0, 15, -15])
        jd.moveJoint(jd.RIGHT_FRONT_ARM, arm_zero_pos-mov)
        jd.moveJoint(jd.LEFT_FRONT_ARM, arm_zero_pos+mov)
        jd.moveJoint(jd.RIGHT_BACK_ARM, arm_zero_pos-mov)
        jd.moveJoint(jd.LEFT_BACK_ARM, arm_zero_pos+mov)
        jd.moveJoint(jd.HEAD, random.choice([0, 3, -3, 0, 2, -2]))
        jd.moveJoint(jd.NECK, random.choice([0, 4, -4, 0, 2, -2]))
        time.sleep(1)
    
    time.sleep(4)
    jd.relax()


#prova i vari passi: avanti, indietro, ruota a destra, ruota a sinistra
if(args.command == "steps"):
    #va avanti
    jd.stepForward(2)
    time.sleep(1)
    #va indietro
    jd.stepBack(2)
    time.sleep(1)
    #gira a sinistra
    jd.stepTurnLeft(2)
    time.sleep(1)
    #gira a destra
    jd.stepTurnRight(2)
    time.sleep(1)
    #fine
    jd.zero(50)
    time.sleep(2)
    jd.zero()
    time.sleep(0.3)
    jd.relax()


#cerca qualcosa intorno a se
if(args.command == "search"):
    arm_zero_pos=50
    offset_neck=0
    jd.zero(arm_zero_pos)
    time.sleep(1)

    for _ in range(4):

        #sceglie la posizione del corpo da assumere in maniera random
        bodyPosition = random.choice(["back-left-weight", "front-right-weight", "back-right-weight", "front-left-weight"])

        #back-left-weight
        if(bodyPosition=="back-left-weight"):
            jd.moveJoint(jd.RIGHT_FRONT_ARM, arm_zero_pos)
            jd.moveJoint(jd.LEFT_FRONT_ARM, arm_zero_pos)
            jd.moveJoint(jd.RIGHT_BACK_ARM, arm_zero_pos-50)
            jd.moveJoint(jd.LEFT_BACK_ARM, arm_zero_pos-70)
            offset_neck = 10

        #front-right-weight
        if(bodyPosition=="front-right-weight"):
            jd.moveJoint(jd.RIGHT_FRONT_ARM, arm_zero_pos-70)
            jd.moveJoint(jd.LEFT_FRONT_ARM, arm_zero_pos-50)
            jd.moveJoint(jd.RIGHT_BACK_ARM, arm_zero_pos)
            jd.moveJoint(jd.LEFT_BACK_ARM, arm_zero_pos)
            offset_neck = -10

        #back-right-weight
        if(bodyPosition=="back-right-weight"):
            jd.moveJoint(jd.RIGHT_FRONT_ARM, arm_zero_pos)
            jd.moveJoint(jd.LEFT_FRONT_ARM, arm_zero_pos)
            jd.moveJoint(jd.RIGHT_BACK_ARM, arm_zero_pos-70)
            jd.moveJoint(jd.LEFT_BACK_ARM, arm_zero_pos-50)
            offset_neck = 10

        #back-right-weight
        if(bodyPosition=="front-left-weight"):
            jd.moveJoint(jd.RIGHT_FRONT_ARM, arm_zero_pos-50)
            jd.moveJoint(jd.LEFT_FRONT_ARM, arm_zero_pos-70)
            jd.moveJoint(jd.RIGHT_BACK_ARM, arm_zero_pos)
            jd.moveJoint(jd.LEFT_BACK_ARM, arm_zero_pos)
            offset_neck = -10
        
        #muove la testa alla ricerca di un volto
        for degreeH in [-15, -20, -25, -30, -35]:
            for degreeW in [-15, 0, 15, -20, 20]:
                #aggiorna la posizione della testa (telecamera)
                jd.moveJoint(jd.HEAD, degreeH)
                jd.moveJoint(jd.NECK, degreeW)

                #cerca una faccia
                detected = dt.detectFace(debug=DEBUG)

                #se trova la faccia la segue
                if detected is not None:        
                    print("found face %s" % (str(detected)))

                    neckDegree = 0
                    headDegree = 0
                    faceRect = detected[-1:]
                    for _ in range(1000):
                        faceRect, neckDegree, headDegree = dt.followFace(neckDegree, headDegree)
                        
                        if(faceRect is None):
                            print("loss face")
                            break

                        time.sleep(0.1)
                else:
                    print("no face found") 

    time.sleep(1)
    jd.zero(arm_zero_pos)
    time.sleep(2)
    jd.zero()
    time.sleep(0.3)
    jd.relax()


#detectface test
if(args.command == "detectface"):
    jd.zero(50)
    jd.moveJoint(jd.HEAD, -20)
    time.sleep(1)
    for i in range(20):
        print("step %s" % i)
        detected = dt.detectFace(debug=DEBUG)
       
        #stampa il risultato
        if detected is not None:        
            print("found face %s" % (str(detected)))
            
            #annuisce con la testa
            for _ in range(5):
                jd.moveJoint(jd.HEAD, -10)
                time.sleep(0.4)
                jd.moveJoint(jd.HEAD, +10)
                time.sleep(0.4)
            jd.moveJoint(jd.HEAD, 0)
            #break

    time.sleep(2)
    jd.zero()
    time.sleep(0.3)
    jd.relax()


#follow face
if(args.command == "followface"):
    jd.zero(50)
    time.sleep(1)
    for i in range(20):
        print("step %s" % i)
        detected = dt.detectFace()

        #stampa il risultato
        if detected is not None:        
            print("found face %s" % (str(detected)))

            neckDegree = 0
            headDegree = 0
            faceRect = detected[-1:]
            for _ in range(1000):
                faceRect, neckDegree, headDegree = dt.followFace(neckDegree, headDegree)
                
                if(faceRect is None):
                    print("loss face")
                    break

                time.sleep(0.1)
        else:
            print("no face found")       

        time.sleep(0.1)

    time.sleep(2)
    jd.zero()
    time.sleep(0.3)
    jd.relax()


#detectball test
if(args.command == "detectball"):
    jd.zero(50)
    jd.moveJoint(jd.HEAD, -20)
    time.sleep(1)
    for i in range(20):
        print("step %s" % i)
        detected = dt.detectBall(debug=True)

        #stampa il risultato
        if detected is not None:        
            print("found ball %s" % (str(detected)))
            
            #annuisce con la testa se trova la palla
            for _ in range(5):
                jd.moveJoint(jd.HEAD, -10)
                time.sleep(0.4)
                jd.moveJoint(jd.HEAD, +10)
                time.sleep(0.4)
            jd.moveJoint(jd.HEAD, 0)
            #break

    time.sleep(2)
    jd.zero()
    time.sleep(0.3)
    jd.relax()


#follow ball
if(args.command == "followball"):
    #detect ball
    arm_zero_pos = 50
    jd.zero(arm_zero_pos)
    time.sleep(1)

    #cicla 20 volte
    for i in range(20):
        print("step %s" % i)
        detected = dt.detectBall()
        
        #se ha tovato la palla..
        if detected is not None:        
            print("found ball %s" % (str(detected)))            
            
            #annuisce con la testa ed esce dal ciclo
            for _ in range(5):
                jd.moveJoint(jd.HEAD, -10)
                time.sleep(0.4)
                jd.moveJoint(jd.HEAD, +10)
                time.sleep(0.4)
            jd.moveJoint(jd.HEAD, 0)
            break

        #si muove leggermente in modo random
        mov = random.choice([0, 15, -15])
        jd.moveJoint(jd.RIGHT_FRONT_ARM, arm_zero_pos-mov)
        jd.moveJoint(jd.LEFT_FRONT_ARM, arm_zero_pos+mov)
        jd.moveJoint(jd.RIGHT_BACK_ARM, arm_zero_pos-mov)
        jd.moveJoint(jd.LEFT_BACK_ARM, arm_zero_pos+mov)
        jd.moveJoint(jd.HEAD, random.choice([0, 3, -3, 0, 2, -2]))
        jd.moveJoint(jd.NECK, random.choice([0, 4, -4, 0, 2, -2]))
        time.sleep(0.1)

    #follow ball
    neckDegree = 3
    headDegree = -20

    #si muove come un cane che vuole giocare
    jd.moveJoint(jd.RIGHT_FRONT_ARM, arm_zero_pos-70)
    jd.moveJoint(jd.LEFT_FRONT_ARM, arm_zero_pos-50)
    jd.moveJoint(jd.RIGHT_BACK_ARM, arm_zero_pos)
    jd.moveJoint(jd.LEFT_BACK_ARM, arm_zero_pos)
    jd.moveJoint(jd.NECK, neckDegree)
    jd.moveJoint(jd.HEAD, headDegree)
    time.sleep(0.2)
    for degree in [50, -50, 50, -50, 50, -50, 50, -50, 0]:
        jd.moveJoint(jd.RIGHT_BACK_SHOULDER, -degree)
        jd.moveJoint(jd.LEFT_BACK_SHOULDER, degree)
        time.sleep(0.2)

    #cicla x 20 volte    
    for i in range(20):
        print("step %s" % i)
        detected = dt.detectBall(debug=DEBUG)

        #se trova la palla..
        if detected is not None:        
            print("found ball %s" % (str(detected)))

            #cicla per seguire la palla
            ballCenter, _ = detected
            loss_count = 0
            for _ in range(10000):
                ballCenter, neckDegree, headDegree = dt.followBall(neckDegree, headDegree, debug=DEBUG)
                
                #se va oltre il range del collo si sposta
                if(headDegree > 30):
                    #se va troppo in basso si sposta in alto
                    jd.moveJoint(jd.HEAD, headDegree-5)
                    jd.moveJoint(jd.RIGHT_FRONT_ARM, arm_zero_pos)
                    jd.moveJoint(jd.LEFT_FRONT_ARM, arm_zero_pos)
                    jd.moveJoint(jd.RIGHT_BACK_ARM, arm_zero_pos)
                    jd.moveJoint(jd.LEFT_BACK_ARM, arm_zero_pos)
                if(neckDegree > 30) : 
                    #se va troppo a destra si sposta a destra
                    neckDegree -= 30
                    jd.stepTurnRight(1)
                    jd.moveJoint(jd.RIGHT_FRONT_SHOULDER, 0)
                    jd.moveJoint(jd.LEFT_FRONT_SHOULDER, 0)
                    jd.moveJoint(jd.RIGHT_BACK_SHOULDER, 0)
                    jd.moveJoint(jd.LEFT_BACK_SHOULDER, 0)
                    jd.moveJoint(jd.RIGHT_FRONT_ARM, arm_zero_pos-70)
                    jd.moveJoint(jd.LEFT_FRONT_ARM, arm_zero_pos-50)
                    jd.moveJoint(jd.RIGHT_BACK_ARM, arm_zero_pos)
                    jd.moveJoint(jd.LEFT_BACK_ARM, arm_zero_pos)
                    jd.moveJoint(jd.NECK, neckDegree)
                    jd.moveJoint(jd.HEAD, headDegree)
                if(neckDegree < -30): 
                    #se va troppo a sinistra si sposta a sinistra
                    neckDegree += 15
                    jd.stepTurnLeft(1)
                    jd.moveJoint(jd.RIGHT_FRONT_SHOULDER, 0)
                    jd.moveJoint(jd.LEFT_FRONT_SHOULDER, 0)
                    jd.moveJoint(jd.RIGHT_BACK_SHOULDER, 0)
                    jd.moveJoint(jd.LEFT_BACK_SHOULDER, 0)  
                    jd.moveJoint(jd.RIGHT_FRONT_ARM, arm_zero_pos-70)
                    jd.moveJoint(jd.LEFT_FRONT_ARM, arm_zero_pos-50)
                    jd.moveJoint(jd.RIGHT_BACK_ARM, arm_zero_pos)
                    jd.moveJoint(jd.LEFT_BACK_ARM, arm_zero_pos)
                    jd.moveJoint(jd.NECK, neckDegree)
                    jd.moveJoint(jd.HEAD, headDegree)
                    
                #se perde la palla incrementa il loss count
                if(ballCenter is None):
                    print("loss ball")
                    loss_count += 1
                else:
                    loss_count = 0

                #se ha perso la palla piu' di 20 volte esce dal ciclo
                if(loss_count > 20):
                    break;

                #time.sleep(0.1)    
        else:
            print("no ball found")       

        time.sleep(0.1)

    #fine
    time.sleep(2)
    jd.zero()
    time.sleep(0.3)
    jd.relax()


#scan head
if(args.command == "scan"):
    jd.zero(50)
    jd.headScanning(0.3)


#pulitura
if(args.command == "relax"):
    jd.relax()


#test gyroscope
if(args.command == "gyro"):
    arm_zero_pos = 50
    jd.zero(arm_zero_pos)
    time.sleep(0.5)

    TILT = 70

    for bodyPosition in ["back-weight", "front-weight", "right-weight", "left-weight", "uniform-weight"]:

        #back-left-weight
        if(bodyPosition=="back-weight"):
            jd.moveJoint(jd.RIGHT_FRONT_ARM, arm_zero_pos)
            jd.moveJoint(jd.LEFT_FRONT_ARM, arm_zero_pos)
            jd.moveJoint(jd.RIGHT_BACK_ARM, arm_zero_pos-TILT)
            jd.moveJoint(jd.LEFT_BACK_ARM, arm_zero_pos-TILT)
            jd.moveJoint(jd.NECK, 0)
            jd.moveJoint(jd.HEAD, 10)

        #front-right-weight
        if(bodyPosition=="front-weight"):
            jd.moveJoint(jd.RIGHT_FRONT_ARM, arm_zero_pos-TILT)
            jd.moveJoint(jd.LEFT_FRONT_ARM, arm_zero_pos-TILT)
            jd.moveJoint(jd.RIGHT_BACK_ARM, arm_zero_pos)
            jd.moveJoint(jd.LEFT_BACK_ARM, arm_zero_pos)
            jd.moveJoint(jd.NECK, 0)
            jd.moveJoint(jd.HEAD, -10)

        #right-weight
        if(bodyPosition=="right-weight"):
            jd.moveJoint(jd.RIGHT_FRONT_ARM, arm_zero_pos-TILT)
            jd.moveJoint(jd.LEFT_FRONT_ARM, arm_zero_pos)
            jd.moveJoint(jd.RIGHT_BACK_ARM, arm_zero_pos-70)
            jd.moveJoint(jd.LEFT_BACK_ARM, arm_zero_pos)
            jd.moveJoint(jd.NECK, -10)
            jd.moveJoint(jd.HEAD, 0)

        #left-weight
        if(bodyPosition=="left-weight"):
            jd.moveJoint(jd.RIGHT_FRONT_ARM, arm_zero_pos)
            jd.moveJoint(jd.LEFT_FRONT_ARM, arm_zero_pos-TILT)
            jd.moveJoint(jd.RIGHT_BACK_ARM, arm_zero_pos)
            jd.moveJoint(jd.LEFT_BACK_ARM, arm_zero_pos-TILT)
            jd.moveJoint(jd.NECK, 10)
            jd.moveJoint(jd.HEAD, 0)

        #uniform-weight
        if(bodyPosition=="uniform-weight"):
            jd.moveJoint(jd.RIGHT_FRONT_ARM, arm_zero_pos)
            jd.moveJoint(jd.LEFT_FRONT_ARM, arm_zero_pos)
            jd.moveJoint(jd.RIGHT_BACK_ARM, arm_zero_pos)
            jd.moveJoint(jd.LEFT_BACK_ARM, arm_zero_pos)
            jd.moveJoint(jd.NECK, 0)
            jd.moveJoint(jd.HEAD, 0)

        rangeX, rangeY, rangeZ = ss.getGyroRange(5, debug=True)
        print("----------- TOTAL RANGES %s -----------" % bodyPosition)
        print("Rotation in X-Axis : %d - %d"  % (rangeX[0], rangeX[1]))
        print("Rotation in Y-Axis : %d - %d"  % (rangeY[0], rangeY[1]))
        print("Rotation in Z-Axis : %d - %d"  % (rangeZ[0], rangeZ[1]))
        print("------------------------------------------")

        time.sleep(0.5)

    #fine
    jd.zero(50)
    time.sleep(2)
    jd.zero()
    time.sleep(0.3)
    jd.relax()


#register
if(args.command == "acquire"):
    jd.zero(50)
    jd.moveJoint(jd.HEAD, 0)
    jd.moveJoint(jd.NECK, 0)
    time.sleep(1)
    
    dt.registerImages(seconds=10)

    time.sleep(0.2)
    jd.zero()
    time.sleep(0.3)
    jd.relax()


#stop della camera
dt.stop()