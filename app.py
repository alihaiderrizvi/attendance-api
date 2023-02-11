from flask import Flask, request, jsonify
app = Flask(__name__)


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
def post_something():
    id = request.args.get('id', None)
    print(id)
    
    # if id:
    #     return jsonify({
    #         "Message": f"Welcome {name} to our awesome API!",
    #         # Add this option to distinct the POST request
    #         "METHOD": "POST"
    #     })
    # else:
    #     return jsonify({
    #         "ERROR": "No name found. Please send a name."
    #     })
    return jsonify({
            "Message": f"Welcome {id} to our awesome API!",
            "METHOD": "POST"
        })


@app.route('/')
def index():
    # A welcome message to test our server
    return "<h1>Welcome to our Attendance API!</h1>"


if __name__ == '__main__':
    # Threaded option to enable multiple instances for multiple user access support
    app.run(threaded=True, port=5000)