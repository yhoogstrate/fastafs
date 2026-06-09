#!/usr/bin/env python3
"""
Start een lokale HTTP-server voor het fastafs benchmark-rapport.

De server serveert de benchmarks/-map en genereert /files.json dynamisch:
alle *.txt-bestanden in de map worden als JSON-array teruggegeven. Zo hoeft
er geen files.json bijgehouden te worden door de benchmark-scripts.

Gebruik (vanuit de repo-root):
    python3 benchmarks/serve_report.py
    python3 benchmarks/serve_report.py --port 9000
    python3 benchmarks/serve_report.py --no-browser

Opent automatisch http://localhost:<port>/report.html in de browser.
Stop met Ctrl+C.
"""

import argparse
import http.server
import json
import os
import webbrowser
from pathlib import Path

BENCH_DIR = Path(__file__).resolve().parent


class Handler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(BENCH_DIR), **kwargs)

    def end_headers(self):
        self.send_header('Cache-Control', 'no-store')
        super().end_headers()

    def do_GET(self):
        if self.path in ('/files.json', '/files.json?'):
            files = sorted(p.name for p in BENCH_DIR.glob('*.txt'))
            body  = json.dumps(files, indent=2).encode()
            self.send_response(200)
            self.send_header('Content-Type', 'application/json; charset=utf-8')
            self.send_header('Content-Length', str(len(body)))
            self.end_headers()
            self.wfile.write(body)
        else:
            super().do_GET()

    def log_message(self, fmt, *args):
        print(f'  {self.address_string()} {fmt % args}')


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--port',       type=int, default=8000, help='poortnummer (standaard 8000)')
    parser.add_argument('--no-browser', action='store_true',    help='open de browser niet automatisch')
    args = parser.parse_args()

    url = f'http://localhost:{args.port}/report.html'
    print(f'fastafs benchmark-rapport  →  {url}')
    print(f'Serveert: {BENCH_DIR}')
    print('Stop met Ctrl+C.\n')

    if not args.no_browser:
        webbrowser.open(url)

    with http.server.HTTPServer(('', args.port), Handler) as httpd:
        httpd.serve_forever()


if __name__ == '__main__':
    main()
