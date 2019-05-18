# CafeLoader Client
# Originally by Kinnay
# with small edits by AboodXD

import os
import socketserver
import struct
import sys

titleID = b''


class TCPHandler(socketserver.BaseRequestHandler):
    def setup(self):
        print('Connection')
        self.files = {}
        self.fhandle = 0x12345678

    def handle(self):
        while True:
            try:
                rawcmd = self.request.recv(1)

            except:
                return
	    
            if not rawcmd:
                return
            
            cmd = ord(rawcmd)
            if cmd == 1:
                global titleID
                titleID = self.request.recv(639)[:16]

                print('Connected to Wii U!. Title ID: %s' % titleID)
                self.request.sendall(struct.pack('>H', 0xCAFE))  # OK
                
            elif cmd == 2:  # Open file
                length = struct.unpack('>I', self.request.recv(4))[0]
                path = self.request.recv(length).lstrip(b'/')

                if path[:4] != b'vol/':
                    path = b''.join([b'vol/content/', path])

                if path[:12] == b'vol/content/':
                    if not titleID:
                        self.request.sendall(struct.pack('>H', 0))

                    path = b''.join([b'vol/', titleID, path[3:]])

                print('FSOpenFile(%s)' %path)
                self.files[self.fhandle] = open(path, 'rb')
                self.request.sendall(struct.pack('>I', self.fhandle))
                self.fhandle += 1

            elif cmd == 3:  # Read file
                print(' - Read')
                handle = struct.unpack('>I', self.request.recv(4))[0]
                size = struct.unpack('>I', self.request.recv(4))[0]
                count = struct.unpack('>I', self.request.recv(4))[0]

                data = self.files[handle].read(size * count)
                self.request.sendall(struct.pack('>I', len(data) // size))
                self.request.sendall(struct.pack('>I', len(data)))
                self.request.sendall(data)

            elif cmd == 4:  # Write file
                print(' - Write')
                handle = struct.unpack('>I', self.request.recv(4))[0]
                length = struct.unpack('>I', self.request.recv(4))[0]

                data = self.recvall(length)
                self.files[handle].write(data)

            elif cmd == 5:  # Close file
                print(' - Close')
                handle = struct.unpack('>I', self.request.recv(4))[0]
                self.files.pop(handle).close()

            elif cmd == 6:  # Save open file
                """
                length = struct.unpack('>I', self.request.recv(4))[0]
                path = self.request.recv(length)
                mode = chr(self.request.recv(1)[0])
                print('SAVEOpenFile(%s, %s)' %(path, mode))
                savepath = b'vol/save/' + path
                if os.path.isfile(savepath) or mode == 'w':
                    self.files[self.fhandle] = open(savepath, mode+'b')
                    self.request.sendall(struct.pack('>I', self.fhandle))
                    self.fhandle += 1
                else:
                    self.request.sendall(b'\x00\x00\x00\x00')
                """
                pass  # TODO

            elif cmd == 7:  # Debug message
                """
                length = struct.unpack('>I', self.request.recv(4))[0]
                message = self.request.recv(length)
                print('DEBUG:', message)
                """
                pass  # do not use this

            elif cmd == 8:  # Get stat file
                print(' - GetStatFile')
                handle = struct.unpack('>I', self.request.recv(4))[0]
                file = self.files[handle]
                pos = file.tell()
                file.seek(0, 2)
                size = file.tell()
                file.seek(pos)
                self.request.sendall(struct.pack('>I', size))

            elif cmd == 9:  # Set pos file
                handle = struct.unpack('>I', self.request.recv(4))[0]
                pos = struct.unpack('>I', self.request.recv(4))[0]
                print(' - SetPosFile(%i)' %pos)
                self.files[handle].seek(pos)

            elif cmd == 10: # Crash report (never actually used by CafeLoader)
                length = struct.unpack('>I', self.request.recv(4))[0]
                report = self.request.recv(length).decode('ascii', 'ignore')
                print(report)

                length = struct.unpack('>I', self.request.recv(4))[0]
                stackTrace = struct.unpack('>' + 'I' * length, self.request.recv(length * 4))
                print('Stack trace:')
                for address in stackTrace:
                    print('\t' + hex(address))

            elif cmd == 11:  # Debug file
                fnlength, length = struct.unpack('>II', self.request.recv(8))
                filename = self.request.recv(fnlength).decode('ascii')
                data = self.recvall(length)

                if not os.path.isdir('DebugFiles'):
                    os.mkdir('DebugFiles')

                with open('DebugFiles/' + filename, 'wb') as f:
                    f.write(data)
            
            elif cmd == 12:  # File check
                length = struct.unpack('>I', self.request.recv(4))[0]
                path = self.request.recv(length).lstrip(b'/')

                if path[:4] != b'vol/':
                    path = b''.join([b'vol/content/', path])  # Fix for NSMBU

                if path[:12] == b'vol/content/':
                    if not titleID:
                        self.request.sendall(struct.pack('>H', 0))

                    path = b''.join([b'vol/', titleID, path[3:]])

                #print("Search for path: %s" % path)
                if os.path.isfile(path):
                    self.request.sendall(struct.pack('>H', 0xCAFE))  # OK

                else:
                    self.request.sendall(struct.pack('>H', 0))
                    
            else:
                print('Invalid command: %i' %cmd)

    def recvall(self, length):
        data = self.request.recv(length)
        while len(data) < length:
            data += self.request.recv(length - len(data))

        return data

    def sendFile(self, fn):
        print('Sending file:', fn)
        with open(fn, 'rb') as f:
            data = f.read()

        self.request.sendall(struct.pack('>I', len(data)))
        self.request.sendall(data)

    def finish(self):
        print('Finish')
        global titleID
        titleID = b''


class TCPServer(socketserver.TCPServer, socketserver.ThreadingMixIn):
    pass


def generateIPBin(ip):
    ipList = list(map(int, ip.split('.')))
    ipNum = 0

    for i, n in enumerate(ipList):
        ipNum |= n << (8 * i)

    with open("ip.bin", "wb") as out:
        out.write(ipNum.to_bytes(4, 'little'))


# ip stores local IP of the computer that has the 
# files by which the game's files should be patched.
ip = input('Enter your PC\'s local IP (e.g. 192.168.1.1): ')
if '--bin' in sys.argv:
    generateIPBin(ip)

server = TCPServer((ip, 2557), TCPHandler)
print('Server has been started')
print('Listening at (%s, 2557)' %ip)
server.serve_forever()
