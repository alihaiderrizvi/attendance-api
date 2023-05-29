def get_student_details(fingerprint_id, attendance_db):

    query = {"fingerprint_id": fingerprint_id}
    result = attendance_db.students.find_one(query, {"roll_no": 1, "name": 1, "class": 1, "father_phone": 1, "mother_phone": 1})
    
    # print(result)
    return result

def fetch_latest_entry(fingerprint_id, attendance_db):
    # Specify the query filter
    query = {"fingerprint_id": fingerprint_id}

    # Sort by REPORTING_TIME in descending order
    sort = [("reporting_time", -1)]

    # Retrieve the last entry
    result = attendance_db.attendance.find_one(query, projection={"name": 1,"date": 1, "reporting_time": 1, "departure_time": 1, "roll_no": 1,}, sort=sort)
    return result


def insert_reporting(fingerprint_id, roll_no, name, current_date, reporting_time, attendance_db):
    document = {
        "fingerprint_id":   fingerprint_id,
        "roll_no":          roll_no,
        "name":             name,
        "date":             current_date,
        "reporting_time":   reporting_time
    }

    result = attendance_db.attendance.insert_one(document)

    # print("Inserted document ID:", result.inserted_id)

def insert_departure(attendance_db, student_id, roll_no, student_name, date, departure_time):
    # Prepare the document to be inserted
    document = {
        "student_id":       student_id,
        "roll_no":          roll_no,
        "student_name":     student_name,
        "date":             date,
        "departure_time":   departure_time
    }

    # Insert the document into the collection
    result = attendance_db.attendance.insert_one(document)
    print("Inserted document ID:", result.inserted_id)

def update_departure(fingerprint_id, current_datetime, attendance_db):

    filter = {"fingerprint_id": fingerprint_id}
    update = {"$set": {"departure_time": current_datetime}}
    result = attendance_db.update_one(filter, update, sort=[("reporting_time", -1)])
    # print("Modified document count:", result.modified_count)
    query = {"fingerprint_id": fingerprint_id}
    
    result = attendance_db.students.find_one(query, {"name": 1})
    student_name = result['name']

    return student_name
