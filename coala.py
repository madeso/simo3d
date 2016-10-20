#!/usr/bin/env python3
import json
import sys

data = json.load(sys.stdin)
for r in data['results']['default']:
  message = r['message']
  ac = r['affected_code'][0]['start']
  file = ac['file']
  line = ac['line']
  print('{0}({1}): {2}'.format(file, line, message))

