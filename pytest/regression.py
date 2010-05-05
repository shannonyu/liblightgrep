#!/usr/bin/env python
# encoding: utf-8
"""
regression.py

Created by Jon Stewart on 2010-05-05.
Copyright (c) 2010 Lightbox Technologies, Inc. All rights reserved.
"""

import sys
import os
import subprocess
import os.path as p
import filecmp

keysDir = './pytest/keys'
docsDir = './pytest/corpora'
resultsDir = './pytest/results'
tempDir = './tmp'

def run(lightgrep, keypath, docpath, temppath):
#  print(command)
  results = os.open(temppath, os.O_CREAT | os.O_TRUNC)
  proc = subprocess.Popen(['time', lightgrep, 'search', keypath, docpath], stdout=results, stderr=subprocess.PIPE)
  cerr = proc.communicate()[1].splitlines()
  rec = {}
  for line in cerr:
    atoms = line.split()
    while (len(atoms) >= 2):
      key = atoms.pop()
      rec[key] = atoms.pop()
  print("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s" % (docpath,
                                                    keypath,
                                                    rec['keywords'],
                                                    rec['vertices'],
                                                    rec['edges'],
                                                    rec['instructions'],
                                                    rec['hits'],
                                                    rec['real'],
                                                    rec['user'],
                                                    rec['sys']))

def main():
  keyfiles = os.listdir(keysDir)
  corpora = os.listdir(docsDir)
  lightgrep = p.abspath('./bin/test/test')
  print("doc\tkeyfile\tkeywords\tvertices\tedges\tinstructions\thits\treal\tuser\tsys")
  if (p.exists(tempDir) == False):
    os.mkdir(tempDir)
  for doc in corpora:
    docpath = p.abspath(p.join(docsDir, doc))
    for k in keyfiles:
      keypath = p.abspath(p.join(keysDir, k))
      temppath = p.abspath(p.join(tempDir, k))
      run(lightgrep, keypath, docpath, temppath)
#      run('%s search %s %s > %s' % (lightgrep, keypath, docpath, resultspath))
  diff = filecmp.dircmp(resultsDir, tempDir)
  if (len(diff.diff_files) > 0 or len(diff.left_only) > 0 or len(diff.right_only) > 0):
    filecmp.dircmp(resultsDir, tempDir).report()

if __name__ == '__main__':
  main()
