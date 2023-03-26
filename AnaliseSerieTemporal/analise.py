

import json
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
import datetime
import time



if __name__ == "__main__":

    json_file = open("bancotestelstmparaanalisedeserietemporal.json")
    dic = json.load(json_file)
   
    arr = []

    for item in dic:
        arr.append(dic[item])


    for item in arr: 
        
        item['Humidity'] = item['Humidity'].split(":")[1].split("}")[0]
        item['Temperature'] = item['Temperature'].split(":")[1].split("}")[0]
        item['Time'] = item['Time'].split(":")[1].split("}")[0].replace('"', '')
        item['pH'] = item['pH'].split(":")[1].split("}")[0]
        item['pHpred'] = item['pHpred'].split(":")[1].split("}")[0]

    #print(arr)

        
dfra = pd.DataFrame(columns=['Humidity', 'Temperature', 'Time', 'pH', 'pHpred'])
dfra = dfra.append(arr, ignore_index=True)
dfra.to_csv('bancotestelstmparaanalisedeserietemporal.csv', index=False)

#print(dfra)

dfra['Humidity'] = dfra['Humidity'].astype(float)
dfra['pH'] = dfra['pH'].astype(float)
dfra['Temperature'] = dfra['Temperature'].astype(float)
dfra['pHpred'] = dfra['pHpred'].astype(float)

 
dfra['Time'] = pd.to_datetime(dfra['Time'], format='%Y-%m-%d %H:%M:%S')

#print(dfra['Time'])

 
humi = dfra['Humidity']
temp = dfra['Temperature']
pH = dfra['pH']
pHpred = dfra['pHpred'].dropna()
time = dfra['Time']

dfra = dfra.dropna() 
#print(time.dtypes)


dfra = dfra[dfra['pH'] > 0] 
dfra = dfra[dfra['pHpred'] > 0]





fig, ax = plt.subplots(figsize=(20, 10))
ax.plot(time, pH, label='pH')
ax.plot(time, pHpred, label='pHpred')
ax.set_xlabel('Time')
ax.set_ylabel('pH')
ax.set_title('pH and pHpred')
ax.legend()
plt.show()



