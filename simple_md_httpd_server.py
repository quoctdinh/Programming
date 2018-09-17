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
			self.send_response( 200 )
			self.send_header('Content-type', 'text/html')
			self.end_headers()

			with open( file_path, 'r' ) as mdfile:
				md = mdfile.read()
				extensions = ['extra', 'smarty']
				html = markdown.markdown(md, extensions=extensions, output_format='html5')
				return self.wfile.write( html )
			
		return SimpleHTTPServer.SimpleHTTPRequestHandler.do_GET(self)

Handler = MyRequestHandler
server  = SocketServer.TCPServer(('0.0.0.0', 8080), Handler)

try:
	server.serve_forever()
except KeyboardInterrupt:
	server.server_close()
