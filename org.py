# -*- coding: utf-8 -*-
from flask import Flask
from flask_compress import Compress
from flask import request
import flask
import json
import uuid
import requests
import re
import os, os.path
import errno
def mkdir_p(path):
    try:
        os.makedirs(path)
    #except OSError as exc: # Python >2.5
    #    if exc.errno == errno.EEXIST and os.path.isdir(path):
    #        pass
    #    else: raise
    except:
        pass

def safe_open_w(path):
    ''' Open "path" for writing, creating any parent directories as needed.
    '''
    mkdir_p(os.path.dirname(path))
    return open(path, 'w')

def write_to_file(path,data):
    try:
        f=safe_open_w(path)
        f.write(data)
        f.close()
    except:
        pass

#from OpenSSL import SSL
#context = SSL.Context(SSL.SSLv23_METHOD)
#context.use_privatekey_file('server.key')
#context.use_certificate_file('server.crt')

app = Flask(__name__)
Compress(app)

def re_url(url):
    return url.replace("http://","https://")
def re_header(header):
    res_header={}
    for key, value in header.items():
        res_header[key]=value
    return res_header
def re_data(data):
    return data
def clean_url(url):
    url=url.replace('https://','')
    p=url.find('?')
    if p != -1:
        url=url[:p]
    return url

@app.route('/', defaults={'path': ''})
@app.route('/<path:path>', methods=['GET'])
def get(path):
    url=re_url(request.url)
    print url
    header=re_header(request.headers)
    write_to_file(os.path.join('data','req_hd',clean_url(url)),str(request.headers))
    if os.path.isfile(os.path.join('static',clean_url(url))):
        file=open(os.path.join('static',clean_url(url)), 'r')
        content=file.read()
        file.close()
        return content
    else:
        if url.find('error') != -1:
            return '{"resultCode":"success"}'
        for k in range(10):
            r = requests.get(url, headers=header)
            content=r.content
            resp = flask.Response(content, status=r.status_code)
            for key, value in r.headers.iteritems():
                if key=="Transfer-Encoding" or key=="Content-Encoding" or key=="Via" or key=="Content-Length":
                    continue
                resp.headers[key]=value
            write_to_file(os.path.join('data','rsp_hd',clean_url(url)),str(resp.headers))
            write_to_file(os.path.join('data','rsp',clean_url(url)),str(r.content))
            print "OK" , len(content) , r.status_code
            if len(content)!=0:
                break
        return resp
@app.route('/', defaults={'path': ''})
@app.route('/<path:path>', methods=['POST'])
def post(path):
    url=re_url(request.url)
    print url
    header=re_header(request.headers)
    write_to_file(os.path.join('data','req_hd',clean_url(url)),str(request.headers))
    data=re_data(request.get_data())
    write_to_file(os.path.join('data','req',clean_url(url)),str(data))
    r = requests.post(url, headers=header, data=data)
    resp = flask.Response(r.content)
    for key, value in r.headers.iteritems():
        if key=="Transfer-Encoding" or key=="Content-Encoding" or key=="Via" or key=="Content-Length":
             continue
        resp.headers[key]=value
    write_to_file(os.path.join('data','rsp_hd',clean_url(url)),str(resp.headers))
    write_to_file(os.path.join('data','rsp',clean_url(url)),str(r.content))
    print "OK" , len(r.content)
    return resp

if __name__ == '__main__':
    app.run(debug=False, host='0.0.0.0', port=80, threaded=True)

