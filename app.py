from flask import Flask, request, jsonify
from pymongo import MongoClient
import os
from dotenv import load_dotenv
import time
from datetime import datetime, date, timedelta
import pytz
from utils import get_time_details, DEPARTURE_MESSAGE_TYPE, ARRIVAL_MESSAGE_TYPE, get_message, create_payload
from DB_connection import attendance_db
from db_functions import get_student_details, fetch_latest_entry, update_departure, insert_reporting

app = Flask(__name__)

load_dotenv()
region = os.getenv("REGION")

@app.route('/mark_attendance/', methods=['POST', 'GET'])
def mark_attendance():
    print("Request Method:", request.method)
    # fetch id
    id = str(request.args.get('id', None))

    # get student details from db
    details = get_student_details(id, attendance_db)

    if details:
        roll_no, \
        name, \
        class_, \
        father_phone, \
        mother_phone = \
        details['roll_no'], \
        details['name'], \
        details['class'], \
        details['father_phone'], \
        details['mother_phone']
    else:
        return {}

    # get current date and time
    current_datetime, current_time, current_date = get_time_details(region)

    # fetch latest entry from attendance
    latest_entry = fetch_latest_entry(id, attendance_db)
    
    if latest_entry:
        latest_entry = latest_entry[0]
        attendance_id, reporting_time, departure_time = latest_entry
        time_diff = current_datetime - reporting_time
        time_diff_min = time_diff.total_seconds() // 60

        # repeated thumbprints - bacha masti krra hai
        if time_diff <= timedelta(minutes=60):
            return {}

        # if difference is within 12 hours, mark departure
        elif time_diff <= timedelta(minutes=720):
            name = update_departure(id, current_datetime, attendance_db)
            message_type = DEPARTURE_MESSAGE_TYPE
        
        # if difference is greater than 12 hours, mark new entrance
        else:
            ACTIVE_QUERY = insert_reporting(id, roll_no, name, current_date, current_datetime, attendance_db)
            # args = (id, roll_no, name, current_date, current_datetime,)
            message_type = ARRIVAL_MESSAGE_TYPE

    else:
        ACTIVE_QUERY = insert_reporting(id, roll_no, name, current_date, current_datetime, attendance_db)
        # args = (id, roll_no, name, current_date, current_datetime,)
        message_type = ARRIVAL_MESSAGE_TYPE
    
    # with connection:
    #     with connection.cursor() as cursor:
    #         cursor.execute(ACTIVE_QUERY, args)
    
    message = get_message(name, message_type)
    payload = create_payload(message, father_phone, mother_phone)

    return payload


@app.route('/')
def index():
    # A welcome message to test our server
    return "<h1>Welcome to our Attendance API!</h1>"


if __name__ == '__main__':
    app.run(threaded=True, port=5000)
    