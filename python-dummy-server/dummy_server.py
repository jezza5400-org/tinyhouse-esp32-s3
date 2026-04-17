from http.server import ThreadingHTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlsplit, parse_qs, unquote
import json
import time
import argparse
from typing import Any, cast

SERVER_IP = "0.0.0.0"
SERVER_PORT = 8080


class DummyDweetServer(ThreadingHTTPServer):
	store: dict[str, list[dict[str, Any]]]

class DummyDweetHandler(BaseHTTPRequestHandler):
	def _store(self):
		server = cast(DummyDweetServer, self.server)
		return server.store

	def _send_json(self, obj, code=200):
		data = json.dumps(obj).encode('utf-8')
		self.send_response(code)
		self.send_header('Content-Type', 'application/json; charset=utf-8')
		self.send_header('Content-Length', str(len(data)))
		self.send_header('Access-Control-Allow-Origin', '*')
		self.end_headers()
		self.wfile.write(data)

	def _store_record(self, thing, content):
		record = {
			"thing": thing,
			"created": time.strftime('%Y-%m-%dT%H:%M:%SZ', time.gmtime()),
			"content": content
		}
		store = self._store().setdefault(thing, [])
		store.append(record)
		content_json = json.dumps(content, separators=(',', ':'), ensure_ascii=False)
		print(f"RECEIVED dweet for '{thing}': {content_json}")
		return record

	def _read_json_body(self):
		length_header = self.headers.get('Content-Length', '0')
		try:
			length = int(length_header)
		except ValueError:
			return None, 'invalid Content-Length'

		if length <= 0:
			return None, 'missing request body'

		raw = self.rfile.read(length)
		try:
			payload = json.loads(raw.decode('utf-8'))
		except Exception as exc:
			return None, f'invalid JSON body: {exc}'

		return payload, None

	def do_GET(self):
		parts = urlsplit(self.path)
		path = unquote(parts.path)
		qs = {k: v[0] for k, v in parse_qs(parts.query).items()}
		# POST-equivalent endpoint used by your Arduino: /dweet/for/<thing>?key=val...
		if path.startswith('/dweet/for/'):
			thing = path[len('/dweet/for/'):]
			record = self._store_record(thing, qs)
			resp = {"this": record, "success": True}
			self._send_json(resp)

		# retrieve latest: /get/latest/dweet/for/<thing>
		elif path.startswith('/get/latest/dweet/for/'):
			thing = path[len('/get/latest/dweet/for/'):]
			store = self._store().get(thing, [])
			if store:
				resp = {"this": store[-1], "success": True}
				self._send_json(resp)
			else:
				self._send_json({"this": None, "success": False}, code=404)

		# retrieve all: /get/dweets/for/<thing>
		elif path.startswith('/get/dweets/for/'):
			thing = path[len('/get/dweets/for/'):]
			store = self._store().get(thing, [])
			resp = {"with": store, "success": True}
			self._send_json(resp)

		else:
			# simple status page
			self._send_json({"status": "dummy dweet server", "path": path})

	def do_POST(self):
		parts = urlsplit(self.path)
		path = unquote(parts.path)

		if not path.startswith('/dweet/for/'):
			self._send_json({"status": "dummy dweet server", "path": path, "success": False}, code=404)
			return

		thing = path[len('/dweet/for/'):]
		payload, err = self._read_json_body()
		if err is not None:
			self._send_json({"error": err, "success": False}, code=400)
			return

		record = self._store_record(thing, payload)
		self._send_json({"this": record, "success": True})

	def log_message(self, format, *args):
		# quiet default logging (still prints received dweets above)
		return

def run(host='0.0.0.0', port=8080):
	server = DummyDweetServer((host, port), DummyDweetHandler)
	server.store = {}  # in-memory store: { thing: [records...] }
	print(f"Dummy dweet server listening on http://{host}:{port}")
	try:
		server.serve_forever()
	except KeyboardInterrupt:
		print("shutting down")
		server.server_close()

if __name__ == '__main__':
	p = argparse.ArgumentParser(description='Dummy dweet.cc-compatible server')
	p.add_argument('--host', default='0.0.0.0')
	p.add_argument('--port', type=int, default=8080)
	args = p.parse_args()
	run(args.host, args.port)