from pymongo import MongoClient
from bson.objectid import ObjectId
import os
# print("DIR CHECK 2",os.getcwd())
# from app.functions import Face_recognize, Read_login_image 
    
client = MongoClient()
#point the client at mongo URI
client = MongoClient("mongodb+srv://developer:Volto_Mango_2022@cluster0.ifegmpw.mongodb.net/?retryWrites=true&w=majority")
# print("client",client) 
print("Mongo connection has been built") 

mydatabase = client['Voltox'] 
# print("my database",mydatabase) 

collections=mydatabase.list_collection_names() 
print("Mongo collections:",collections)  

user_name = "iqra"
email = "iqra.shafat06gmail.com" 