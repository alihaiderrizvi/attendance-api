from pymongo import MongoClient
import os
from dotenv import load_dotenv

load_dotenv()
mongo_url = os.getenv("CONNECTION_STRING")
client = MongoClient(mongo_url)
attendance_db = client['attendenceApi']
collections=attendance_db.list_collection_names()