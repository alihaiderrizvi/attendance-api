from flask import Flask, request, jsonify
app = Flask(__name__)
import time
from datetime import datetime
import pytz

region = 'Asia/Karachi'

# @app.route('/getmsg/', methods=['GET'])
# def respond():
#     # Retrieve the name from the url parameter /getmsg/?name=
#     name = request.args.get("name", None)

#     # For debugging
#     print(f"Received: {name}")

#     response = {}

#     # Check if the user sent a name at all
#     if not name:
#         response["ERROR"] = "No name found. Please send a name."
#     # Check if the user entered a number
#     elif str(name).isdigit():
#         response["ERROR"] = "The name can't be numeric. Please send a string."
#     else:
#         response["MESSAGE"] = f"Welcome {name} to our awesome API!"

#     # Return the response in json format
#     return jsonify(response)


@app.route('/mark_attendance/', methods=['POST'])
def mark_attendance():
    id = request.args.get('id', None)
    timezone = pytz.timezone(region)
    current_time = datetime.now(timezone)

    # put entry in DB
    # generate message string to be sent to parents
    # if difference between current time and already existing reporting time is less than 3 hrs, do nothing
    # if difference between current time and already existing reporting time is greater than 3 hrs, mark departure time
    # if date changes, mark reporting time

    print(id, timezone, current_time, type(current_time))

    return {
            "Message": f"Welcome {id} to our awesome API!",
            "METHOD": "POST",
            "TIME": current_time
        }


@app.route('/')
def index():
    # A welcome message to test our server
    return "<h1>Welcome to our Attendance API!</h1>"


if __name__ == '__main__':
    app.run(threaded=True, port=5000)