from flask import Flask, request, jsonify
import os
import psycopg2
from dotenv import load_dotenv
import time
from datetime import datetime, date
import pytz
from utils import *

app = Flask(__name__)

load_dotenv()
database_url = os.getenv("DATABASE_URL")
region = os.getenv("REGION")

connection = psycopg2.connect(database_url)

@app.route('/mark_attendance/', methods=['POST'])
def mark_attendance():
    # fetch id
    id = request.args.get('id', None)

    # get student details from db
    details = get_student_details(id, connection)

    if details:
        roll_no, name, class_, father_phone, mother_phone = res
    else:
        return

    # get current date and time
    current_datetime, current_time, current_date = get_time_details(region)

    # fetch latest entry from attendance
    latest_entry = fetch_latest_entry(id, connection)
    
    if latest_entry:
        latest_entry = latest_entry[0]
        attendance_id, reporting_time, departure_time = latest_entry
        time_diff = current_datetime - reporting_time
        min_in_day = 24 * 60
        time_diff_min = time_diff.total_seconds() // 60

        # repeated thumbprints - bacha masti krra hai
        if time_diff <= 60:
            return

        # if difference is within 12 hours, mark departure
        elif time_diff <= 720:
            ACTIVE_QUERY = UPDATE_DEPARTURE
            args = (current_datetime, id,)
            message_type = DEPARTURE_MESSAGE_TYPE
        
        # if difference is greater than 12 hours, mark new entrance
        else:
            ACTIVE_QUERY = INSERT_ENTRANCE
            args = (id, roll_no, name, current_date, current_datetime,)
            message_type = ARRIVAL_MESSAGE_TYPE

    else:
        ACTIVE_QUERY = INSERT_ENTRANCE
        args = (id, roll_no, name, current_date, current_datetime,)
        message_type = ARRIVAL_MESSAGE_TYPE
    
    with connection:
        with connection.cursor() as cursor:
            cursor.execute(ACTIVE_QUERY, args)
    
    message = get_message(name, message_type)
    payload = create_payload(message, father_phone, mother_phone)

    return payload


@app.route('/')
def index():
    # A welcome message to test our server
    return "<h1>Welcome to our Attendance API!</h1>"


if __name__ == '__main__':
    app.run(threaded=True, port=5000)