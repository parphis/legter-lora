from datetime import datetime
import json

def get_timestamp():
    return datetime.now().strftime(("%Y-%m-%d %H:%M:%S"))


# Create a handler for our read (GET) people
def getLopy(lopy_id):
    """
    This function responds to a request for /api/getLopy

    :return:        string
    """
    msgs = ''
    lines = []
    with open('/var/www/messages/msgs.lst', mode='r', encoding='utf-8', errors='ignore') as fd:
        ln = fd.readline()
        while ln:
            if lopy_id in ln:
                lines.append(ln)
            ln = fd.readline()
    msgs = '[' + ','.join(lines) + ']'
    #print(msgs)
    return json.loads('{"result":"no entries found"}') if msgs == '[]' else json.loads(msgs)

def sendMessage(lopy_id, msg):
    # {"id":1,"to":"255255255255","timestamp":123456789,"message":"Ez mindenkinek szól! Az árvítűrő tükörfúrógépek ÉPPENCSAK HOGY ÁLLÍTÓLAG CSŐSTÜL ÚJRA AZ ŰRBŐL JÖNNEK...c[_] vagy ---(---(@ valaki? (^`´^)"}
    line = '''{{"id":1,"to":"{0}","timestamp":123456789,"message":"{1}"}}'''.format(lopy_id, msg)
    
    with open('/var/legiot/request.req', 'w') as fd:
        fd.write(line)
    
    return json.loads('{"result":"success"}')
