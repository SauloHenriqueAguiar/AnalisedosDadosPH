import sqlite3
import serial
import datetime
import time
import pandas as pd
import csv
from datetime import datetime


# copie a porta do seu editor Arduino

PORT = 'COM4'
ser = serial.Serial('COM4', 9600, timeout=2)

humArray = []
tempArray = []
timeArray = []

while True:
    message = ser.readline()
    data = message.strip().decode('utf8')
    split_string = data.split(',')  # split string
    # converter a primeira parte da string em float
    humidity = float(split_string[0])
    # converter a segunda parte da string em float
    temperature = float(split_string[1])
    now = datetime.now()
    dt_string = now.strftime("%Y-%m-%d %H:%M:%S")
    # print(temperature)
    print(dt_string, humidity, temperature)
    humArray.append(humidity)  # adicionar valores de umidade ao array
    tempArray.append(temperature)  #adicionar valores de temperatura do array
    timeArray.append(dt_string)
    # print(humArray)
    # print(tempArray)
    # print(timeArray)
    with open("humidity_and_temperature.csv", "a") as f:
        writer = csv.writer(f, delimiter=",")
        # writer.writerow(['Time', 'Humidity', 'Temperature'])
        writer.writerow([dt_string, humidity, temperature])

    df = pd.DataFrame(
       {'Time': timeArray, 'Humidity': humArray, 'Temperature': tempArray})
    print(df)
    '''
    df.to_csv('humidity_and_temperature.csv', mode='a', header=False)
    df1 = pd.read_csv('humidity_and_temperature.csv')
    print(df1)
    '''
    '''
    header_list = ['Time', 'Humidity', 'Temperature']
    df1 = pd.read_csv('humidity_and_temperature.csv', names=header_list)
    print(df1['Temperature'].iloc[-1])
    '''
    # database = r"C:\Users\saulo\Frontpage\db\teste.db"
    # sqliteConnection = sqlite3.connect(database)
    # cursor = sqliteConnection.cursor()

    # Adicionar dados no banco de dados sqlite
    header_list = ['Time', 'Humidity', 'Temperature']
    data = pd.read_csv('humidity_and_temperature.csv', names=header_list)
    # Primeiro crie o nome do banco de dados no sqlite workbench 'humidity_and_temperature'
    database = r"C:\Users\saulo\Frontpage\db\teste.db"
    conn = sqlite3.connect(database)
    '''conn = mysql.connect(host='localhost',
                  database='humidity_and_temperature',
                     user='root',
                     password='sql_root_45t6')
     '''
    cursor = conn.cursor()

    # # Descarte a tabela se a tabela de nomes de poluição existir no banco de dados
    cursor.execute('DROP TABLE IF EXISTS HumidityTemperature;')
   
   
   # Criar instrução para uma tabela
    criatabela = ''' CREATE TABLE HumidityTemperature(
                   Time DATETIME, 
                   Humidity FLOAT, 
                   Temperature FLOAT 

    );'''
    
    
    cursor.execute(criatabela)
    # # Adicione os dados do arquivo csv na tabela sqlite
    for row in data.itertuples():  # itertuples() função é usada para iterar linhas de tupla
        cursor.execute("""
                        INSERT INTO HumidityTemperature(Time, Humidity, Temperature)
                        VALUES(?,?,?)
                        """,
                       (row.Time,
                        row.Humidity,
                        row.Temperature,
                        ))

    print('Records are adding into table ..........')
    conn.commit()
