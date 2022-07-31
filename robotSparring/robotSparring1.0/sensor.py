import smbus
import time

#return Gyroscope values (x, y, z)
def getGyroValues(debug=False):
    # Get I2C bus
    bus = smbus.SMBus(1)

    # L3G4200D address, 0x68(104)
    # Select Control register1, 0x20(32)
    #		0x0F(15)	Normal mode, X, Y, Z-Axis enabled
    bus.write_byte_data(0x69, 0x20, 0x0F)
    # L3G4200D address, 0x68(104)
    # Select Control register4, 0x23(35)
    #		0x30(48)	Continous update, Data LSB at lower address
    #					FSR 2000dps, Self test disabled, 4-wire interface
    bus.write_byte_data(0x69, 0x23, 0x30)

    time.sleep(0.5)

    # L3G4200D address, 0x68(104)
    # Read data back from 0x28(40), 2 bytes, X-Axis LSB first
    data0 = bus.read_byte_data(0x69, 0x28)
    data1 = bus.read_byte_data(0x69, 0x29)

    # Convert the data
    xGyro = data1 * 256 + data0
    if xGyro > 32767 : xGyro -= 65536

    # L3G4200D address, 0x68(104)
    # Read data back from 0x2A(42), 2 bytes, Y-Axis LSB first
    data0 = bus.read_byte_data(0x69, 0x2A)
    data1 = bus.read_byte_data(0x69, 0x2B)

    # Convert the data
    yGyro = data1 * 256 + data0
    if yGyro > 32767 : yGyro -= 65536

    # L3G4200D address, 0x68(104)
    # Read data back from 0x2C(44), 2 bytes, Z-Axis LSB first
    data0 = bus.read_byte_data(0x69, 0x2C)
    data1 = bus.read_byte_data(0x69, 0x2D)

    # Convert the data
    zGyro = data1 * 256 + data0
    if zGyro > 32767 : zGyro -= 65536

    # Output data to screen
    if(debug):
        print("Rotation in X-Axis : %d" %xGyro)
        print("Rotation in Y-Axis : %d" %yGyro)
        print("Rotation in Z-Axis : %d" %zGyro)
    
    #return values
    return (xGyro, yGyro, zGyro)

#torna il range di valori dopo avere fatto un test in base al numero di cicli
def getGyroRange(cycles=10, debug=False):
    
    valueMin = [ 1000, 1000, 1000]
    valueMax = [-1000,-1000,-1000]

    for _ in range(cycles):
        value = getGyroValues()        
        for i in range(3):
            if value[i] < valueMin[i]: valueMin[i] = value[i]
            if value[i] > valueMax[i]: valueMax[i] = value[i]

        if(debug):
            print("Rotation in X-Axis : %d" %value[0])
            print("Rotation in Y-Axis : %d" %value[1])
            print("Rotation in Z-Axis : %d" %value[2])
            print("---------------------------------")
        
        time.sleep(1)
    
    return ((valueMin[0], valueMax[0]), 
            (valueMin[1], valueMax[1]), 
            (valueMin[2], valueMax[2]))