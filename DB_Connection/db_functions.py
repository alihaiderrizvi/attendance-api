

def add_data(user_name, email):  
    mydatabase.users.insert_one({"user_name":user_name, "email": email})     
    print("Document added to mongo succesfully") 

# add_data(user_name, email)  

#add document 
def add_testing_merchant_user_data(analysis_id,user_name):  
    mydatabase.merchant_user.insert_one({"_id":analysis_id, "user_name":user_name})     
    print("Merchant_user Document added to mongo succesfully") 

# add_merchant_user_data(analysis_id, user_name) 

def update_ocr_image(analysis_id,image, cardside):   
    try:
        mydatabase.users.update_one({'_id': ObjectId(analysis_id)} , {'$set': { cardside: image}})   
        print("merchant user ocr image updated to mongo succesfully")  
        return True
    except Exception as e:
        print("Exception in update_ocr_image ", str(e))
        return False

def update_merchant_user_data(analysis_id,enc_values_dict,key_name,cardName):   
    result = mydatabase.users.update_one({'_id': ObjectId(analysis_id)} , {'$set': { key_name: enc_values_dict, "docName": cardName}})    
    print("merchant user ocr data updated to mongo succesfully")  
    return result 

def update_credit_card_data(analysis_id,enc_values_dict,key_name,cardName):   
    result = mydatabase.users.update_one({'_id': ObjectId(analysis_id)} , {'$set': { key_name: enc_values_dict, "cardName": cardName}})    
    print("merchant user ocr data updated to mongo succesfully")  
    return result 

def add_ocr_key(analysis_id,ocr_key,key_name):  
    doc_key = mydatabase.keys.insert_one({"userID":analysis_id, key_name: ocr_key})  
    print("doc_key: ", doc_key.inserted_id) 
    analysis_id = doc_key.inserted_id   
    print("User ocr key document has been added to mongo succesfully")  
    return str(analysis_id)  

def check_key_doc(analysis_id,ocr_key,key_name): 

    doc_id = None

    print("analysis_id: ", analysis_id) 
    user_key_doc = mydatabase.keys.find({}, {"userID" : 1, "_id": 1})    

    for i in user_key_doc:  
        # print("i: ", i) 
        value = i['userID'] 

        if value == analysis_id: 
            print("Document found!") 
            doc_id = i['_id'] 
            print("doc_id: ", doc_id)
            # print("type of doc_id: ", type(doc_id))  
            doc_id=str(doc_id)
            result = mydatabase.keys.update_one({'_id': ObjectId(doc_id)} , {'$set': {key_name: ocr_key}})      
            print("OCR Key has been updated to mongo")  
            return True, doc_id 
    else: 
        return False, doc_id 
 

def add_credit_key(analysis_id,ocr_key,key_name):   
    result = mydatabase.keys.update_one({'_id': ObjectId(analysis_id)} , {'$set': {key_name: ocr_key}})  
    print("User credit card key document has been updated to mongo succesfully")  

# analysis_id = "62fba16238455cc7b5cd1be8"  
# ocr_key = 'QkI2cVVKSVNIeDhEazE4M2tBNW13c0hyNC1IZVoxSjZVc3lfWjlYaXlKbz0=' 

# add_ocr_key(analysis_id,ocr_key) 

def search_ocr_key(analysis_id,card): 

    if card == "id": 
        search_key = "ocrKey"
    if card == "credit":
        search_key = "cardKey" 

    print("analysis_id: ", analysis_id) 
    key = None 

    user_key_doc = mydatabase.keys.find({}, {"userID" : 1, "ocrKey" : 1, "cardKey": 1})  

    for i in user_key_doc:  
        print("i: ", i) 
        value = i['userID'] 
        if card == "id": 
            if "ocrKey" in i:
                ocr_key = i['ocrKey'] 
        if card == "credit": 
            if "cardKey" in i: 
                card_key = i['cardKey']  
        if value == analysis_id: 
            if card == "id": 
                print("id card: ", ocr_key)  
                key = ocr_key 
                print("ocr key found")
            if card == "credit": 
                print("credit_key: ", card_key)  
                key = card_key 
                print("credit card key found") 
            else: 
                pass 
        else: 
            print("key not found") 
            pass 

    return key 

# result = search_ocr_key(analysis_id) 
# print("result", result)

# analysis_id = "6302e7c04feaa1f58d64a5a0"   

def search_ocr_dict(analysis_id): 
    user_ocr_doc = mydatabase.users.find_one({"_id": ObjectId(analysis_id)})   
    print("User image doc type",type(user_ocr_doc))   
    if "ocrData" in user_ocr_doc:
        # print("ocrData: ",user_ocr_doc["ocrData"])  
        ocr_dict = user_ocr_doc["ocrData"] 
    else: 
        print("ocr data not found") 
        ocr_dict = {} 
        
    return ocr_dict  

# result =search_ocr_dict(analysis_id) 
# print("result", result)
# print("type: ", type(result)) 

def search_credit_dict(analysis_id): 
    user_card_doc = mydatabase.users.find_one({"_id": ObjectId(analysis_id)})   
    print("user doc type",type(user_card_doc))   
    if "cardData" in user_card_doc:
        # print("ocrData: ",user_ocr_doc["ocrData"])  
        card_dict = user_card_doc["cardData"] 
    else: 
        print("ocr data not found") 
        card_dict = {} 
        
    return card_dict  

def result_status(analysis_id,status,key_name):
    mydatabase.users.update_one({'_id': ObjectId(analysis_id)} , {'$set': {key_name : status}}) 
    print("API status has been uploaded to Mongo Doc Succesfully")  

def retrieve_user_registration_doc(unique_id):
    try:
        user_doc = mydatabase.users.find_one({'_id': ObjectId(unique_id)})  
        print("user_doc: ", user_doc["registrationFaceImage"]) 
        return user_doc
    except Exception as e:
        print("Error in retrieve_user_registration_doc " , str(e))