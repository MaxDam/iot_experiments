#sudo apt-get update 
#sudo apt-get install libatlas-base-dev
#sudo pip install scikit-learn --index-url https://piwheels.org/simple
#sudo apt install python-sklearn

import numpy as np
import cv2
import os

from sklearn import preprocessing
from sklearn.ensemble import RandomForestRegressor
from sklearn.model_selection import train_test_split
from sklearn.metrics import mean_squared_error, mean_absolute_error, r2_score
from pickle import dump, load

#return image input histogram (Cumulative Distribution Function)
def getImageHistogram(inputImage, inputSize):
    hist, _ = np.histogram(inputImage.flatten(), inputSize, [0,256])
    cdf = hist.cumsum()
    cdf_normalized = cdf * hist.max()/ cdf.max()
    #return hist
    return cdf_normalized

#save data
def saveData(name, X, Y):
    #append data to csv
    csvFileName = "train/" +name + "_data.csv"
    with open(csvFileName, mode='a+') as csvFile:
        row = np.hstack([X, Y])
        csvFile.write(",".join(row.astype(str)) + os.linesep)

#save image data
def saveDataFromImg(name, image, Y):
    #get X from image
    X = getImageHistogram(image)
    #append image data to csv
    writeDataRowToCsv(name, X, Y)

#train the model
def train(name, outputSize=1):
    print("TRAINING MODEL...")

    #read data from csv
    data = []
    csvFileName = "train/" +name + "_data.csv"
    with open(csvFileName, 'r') as csvFile:
        for row in csvFile:
            row = row.replace(os.linesep, '')
            data.append(row.split(","))
    data = np.array(data)
 
    #shuffle and split data
    np.random.shuffle(data)
    X = data[:,:-outputSize]
    Y = data[:,-outputSize:]
    X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.33, random_state=42)
    
    #train model
    scaler = preprocessing.StandardScaler()
    X_train = scaler.fit_transform(X_train)
    X_test = scaler.transform(X_test)
    model = RandomForestRegressor(n_estimators=100, max_depth=30, random_state=0)
    model.fit(X_train, Y_train)
    
    '''
    #verify model
    Y_prediction_train = model.predict(X_train)
    rSquareTrain = r2_score(Y_train, Y_prediction_train)
    maeTrain = mean_absolute_error(Y_train, Y_prediction_train)
    rmseTrain = mean_squared_error(Y_train, Y_prediction_train)**0.5
    Y_prediction_test = model.predict(X_test)
    rSquareTest = r2_score(Y_test, Y_prediction_test)
    maeTest = mean_absolute_error(Y_test, Y_prediction_test)
    rmseTest = mean_squared_error(Y_test, Y_prediction_test)**0.5
    print("-------------------------------")
    print("MAE-TRAIN: ", maeTrain, " (> 0)")
    print("MAE-TEST: ", maeTest, " (> 0)")
    print("-------------------------------")
    print("RMSE-TRAIN: ", rmseTrain, " (> 0)")
    print("RMSE-TEST: ", rmseTest, " (> 0)")
    print("-------------------------------")
    print("R^2-TRAIN: ", rSquareTrain, " (> 1)")
    print("R^2-TEST: ", rSquareTest, " (> 1)")
    print("-------------------------------")
    '''

    # save the model
    dump(model, open("train/" +name+"_model.pkl", "wb"))
    # save the scaler
    dump(scaler, open("train/" +name+"_scaler.pkl", "wb"))

#predict
def predict(name, x):
    # load the model
    model = load(open("train/" +name+"_model.pkl", "rb"))
    # load the scaler
    scaler = load(open("train/" +name+"_scaler.pkl", "rb"))
    #transform scaler
    X = scaler.transform([x])
    #predict
    predict = model.predict(X)
    #return predict
    return predict

#predict from image
def predictFromImg(name, image, inputSize):
    #get input from image
    x = getImageHistogram(name, image, inputSize)
    #predict
    return predict(x)


saveData("test", [10,20,4], 2)
saveData("test", [4,5,40], 1)
train("test", 1)
print(predict("test", [10,20,4])[0])
print(predict("test", [4,5,40])[0])
