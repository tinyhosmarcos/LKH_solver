import os
import pandas as pd

def createDataset(numCities, batch):
  counter = 0
  for i,chunk in enumerate(pd.read_csv('cities.csv', chunksize=numCities)):
    chunk.to_csv('dataset/chunk{}.csv'.format(i), index=False)
    counter += 1
    if(counter == batch):
      break

createDataset(100, 16)