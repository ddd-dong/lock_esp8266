from pymongo import MongoClient
import datetime

url = "mongodb+srv://tst:D1S6yHVKzPW6Sng3@cluster0.wbztfhw.mongodb.net/?retryWrites=true&w=majority"
golbal_permission_list = ("admin","user","guest")

class User(): 
    def __init__(self,_data:dict):
        self.uid = _data["UID"]
        self.id=_data["tst"]
        self.permission=_data["permission"]
        self.description=_data["description"]
        self.invaild_time=_data["invaild_time"]
        self.data=_data

class DB():
    def __init__(self,_url) -> None:
        self.client = MongoClient(_url,tls=True,tlsAllowInvalidCertificates=True)
        self.db = self.client['ckcsc_locker']
        self.collection = self.db['id']
        self.permission_list = golbal_permission_list
    
    def add_user(self,uid:str,id:str,permission:str,description:str="",invaild_time:datetime.datetime=None)->None:
        """
        "UID":"BD 31 15 2B",
        "id":"tst",
        "permission":"admin",
        "description":"test",
        "invaild_time":None
        """
        if permission not in self.permission_list: print(f"Warning! permission name error,{permission} not in {self.permission_list}")
        _data = {"UID":uid,
                 "id":id,
                 "permission":permission,
                 "description":description,
                 "invaild_time":invaild_time}
        self.collection.insert_one(_data)

    def load_user(self,uid:str)->dict or None:
        if not uid: 
            return None
        _data = self.collection.find_one({"UID":uid})
        if not _data:
            return None
        return _data

# -------tst-------
if __name__ == "__main__":
    db= DB(url)
    # db.add_user("f93d47b3","tst","admin","test",None)
    print(db.collection.find_one())
    print(db.load_user("sfsdfsdf"))
