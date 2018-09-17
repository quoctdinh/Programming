#!/usr/bin/env python
import SimpleHTTPServer
import SocketServer
import markdown

class MyRequestHandler(SimpleHTTPServer.SimpleHTTPRequestHandler):
    def do_GET(self):
        file_path = self.path
        file_ext  = file_path.split( '.' )[-1]
        file_path = file_path.replace( '/', '' )

        if file_ext == 'md':
            html = ''
            try:
                with open( file_path, 'r' ) as mdfile:
                    md   = mdfile.read()
                    extensions = ['extra', 'smarty']
                    html = markdown.markdown(md, extensions=extensions, output_format='html5')
            except:
                pass

            if html:
                self.send_response( 200 )
                self.send_header('Content-type', 'text/html')
                self.end_headers()

                return self.wfile.write( html )

        return SimpleHTTPServer.SimpleHTTPRequestHandler.do_GET(self)


class MyTCPServer(SocketServer.TCPServer):
    def server_bind(self):
        import socket
        self.socket.setsockopt( socket.SOL_SOCKET, socket.SO_REUSEADDR, 1 )
        self.socket.bind(self.server_address)


Handler = MyRequestHandler
server  = MyTCPServer(( '0.0.0.0', 8080 ), Handler )


try:
    server.serve_forever()
except KeyboardInterrupt:
    pass

server.server_close()
server.shutdown()
