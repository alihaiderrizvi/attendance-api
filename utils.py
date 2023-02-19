import pytz
from datetime import datetime, date

ARRIVAL_MESSAGE_TYPE = 1
DEPARTURE_MESSAGE_TYPE = 2

#add local time in message
ARRIVAL_MESSAGE = """
Dear Parent,
This is to inform you that %s has arrived at the center.
"""

#add local time in message
DEPARTURE_MESSAGE = """
Dear Parent,
This is to inform you that %s has departed from the center.
"""

STUDENT_DETAILS = """
                    SELECT ROLL_NO, NAME, CLASS, FATHER_PHONE, MOTHER_PHONE
                    FROM PUBLIC.STUDENTS
                    WHERE ID = %s;
                """

LAST_ENTRY = """
                SELECT ID, REPORTING_TIME, DEPARTURE_TIME
                FROM PUBLIC.ATTENDANCE
                WHERE STUDENT_ID = %s
                ORDER BY REPORTING_TIME DESC
                LIMIT 1;
            """

INSERT_ENTRANCE = """
                    INSERT INTO PUBLIC.ATTENDANCE (STUDENT_ID, ROLL_NO, STUDENT_NAME, DATE_, REPORTING_TIME)
                    VALUES (%s, %s, %s, %s, %s);
                """

INSERT_DEPARTURE = """
                    INSERT INTO PUBLIC.ATTENDANCE (STUDENT_ID, ROLL_NO, STUDENT_NAME, DATE_, DEPARTURE_TIME)
                    VALUES (%s, %s, %s, %s, %s);
                """

UPDATE_DEPARTURE = """
                    UPDATE PUBLIC.ATTENDANCE
                    SET DEPARTURE_TIME = %s
                    WHERE ID = (
                                SELECT MAX(ID)
                                FROM PUBLIC.ATTENDANCE
                                WHERE STUDENT_ID = %s
                            );
                """

def get_student_details(id, connection):
    with connection:
        with connection.cursor() as cursor:
            cursor.execute(STUDENT_DETAILS, (id,))
            res = cursor.fetchone()
    
    if res:
        roll_no, name, class_, father_phone, mother_phone = res
    else:
        res = None
    
    return res

def get_time_details(region):
    timezone = pytz.timezone(region)
    current_datetime = datetime.now(timezone)
    current_time = current_datetime.strftime("%H:%M:%S")
    current_date = current_datetime.date()

    return current_datetime, current_time, current_date

def fetch_latest_entry(id, connection):
    with connection:
        with connection.cursor() as cursor:
            cursor.execute(LAST_ENTRY, (id,))
            res = cursor.fetchall()
    
    return res

def get_message(name, message_type):
    if message_type == ARRIVAL_MESSAGE_TYPE:
        message = ARRIVAL_MESSAGE % name
    else:
        message = DEPARTURE_MESSAGE % name
    
    return message

def create_payload(message, father_phone, mother_phone):
    numbers = []
    for i in father_phone:
        if (i.startswith('0') and len(i) == 11) or (i.startswith('+92') and len(i) == 13) or (i.startswith('92') and len(i) == 12):
            numbers.append(i)

    for i in mother_phone:
        if (i.startswith('0') and len(i) == 11) or (i.startswith('+92') and len(i) == 13) or (i.startswith('92') and len(i) == 12):
            numbers.append(i)
    
    payload = {
        "message": message,
        "numbers": numbers
    }
    
    return payload


# def insert_dummy_data():
#     timezone = pytz.timezone(region)

#     student_id = 1
#     roll_no = "T9-01"
#     st_name = "Rooh Allah"
#     datee = date.today() # add timezone
#     reporting_time = datetime.now(timezone)
    
#     with connection:
#         with connection.cursor() as cursor:
#             cursor.execute(INSERT_ENTRANCE, (student_id, roll_no, st_name, datee, reporting_time,))