#!/usr/bin/python

# Copyright (c) 2015, Alexander Haase
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of [project] nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


'''Not-quite-C facilitates modern C. 

Specifically, it makes macros workable and attributable. Why? For D.R.Y., 
maintainable code bases. Declarative, D.R.Y. programming is a choice. Only you
can save yourself. Remember:

  - Every token adds potential for error.
  - Copying code propagates error and impeeds tracebility.
  - Consistency absolves us from boilerplate and leads towards metaprograming.
  - Coverage is a convenant: without it there can be no trust.
  - Repeatability is the only antidote for insanity and supersition.

Also recommended:

  - Cleverness in moderation: Too much conciseness obfuscates, as does too 
    little.
  - Codify and re-use concepts: Concept-oriented code leads toward flexibility,
    reusability, which encourages D.R.Y.-first coding.

Manifesto aside, here's some concrete goals:

  - Human- and tool-friendly macro expansion. Limiting macros to one line was 
    never a good idea. Rather than introduce a new template language, we 
    enhance macros by expanding them on multiple lines, with attribution and
    tracibility, before the compiler/preprocessor strip away all the human 
    frils. Suddenly, a tracible intermediate file that is debuggable, without
    modifying the language in any way.

  - Pragma-based macro expansion. Sometimes we don't want to expand macros,
    sometimes we do, and sometimes we want to do it differently.
'''

import sys
import re
import itertools

class Macro( object ):
	'''Representation of a macro...'''

	def __init__( self, lines, number ):
		self.lines = lines
		self.number = number
		tokens = self.tokenize( lines[ 0 ] )

		# identify the macro a bit
		self.identifier = tokens[ 1 ]
		self.isFunction = tokens[ 2 ] == '('
	
		first = lines[ 0 ]

		#extract args
		if self.isFunction:
			argsWithSpecial = itertools.takewhile( lambda x: x != ')', tokens[ 2: ] )
			filteredArgs = itertools.ifilter( lambda x: x not in self.special, argsWithSpecial )

			self.args = list( filteredArgs )
			
			remainder = first[ first.index( ')' ) + 1 : ]
		else:
			self.args = []
			remainder = first[ first.index( self.identifier ) : ]

		#create a format macro
		corpus = [ remainder ]
		corpus.extend( lines[ 1: ] )
		text = ''.join( corpus ).replace( '\\\n', '\n' )
		if self.isFunction:
			for index in xrange( 0, len( self.args ) ):
				arg = self.args[ index ]
				regex = re.compile( '([\W\s])({0})([\W\s])'.format( arg ) )
				replace = lambda match: "{0}{{{1}}}{2}".format( *match.groups( ( 1, 2, 3 ) ) )
				print  r'\n'.join( text.splitlines() )
				text = regex.sub( replace, text )
				print arg
				print '([\W\s])({0})([\W\s])'.format( arg )
				print  r'\n'.join( text.splitlines() )	 + '\n'
			print 'Done ' + r'\n'.join( text.splitlines() )
			self.format = lambda args: text.format( *args )
		else:	
			self.format = lambda args: text
		

	special = '(),[]={}.-*+/^&|!'

	@classmethod
	def tokenize( cls, line ):
		for sym in cls.special:
			line = line.replace( sym, ' {0} '.format( sym ) )
		return line.split()

	def __repr__( self ):
		if self.isFunction:
			return '{0}( {1} )'.format( self.identifier, ', '.join( self.args ) )
		else:
			return self.identifier


class Translator( object ):
	'''Root object for marking up the source. Very line oriented.'''

	def __init__( self, includeHandlers = [] ):
		'''Sets up an empty translator'''
		self.includeHandlers = []
		self.macros = {}
		self.lines = None
		self.lineNumber = 0

	def isMacro( self, line ):
		'''say if line is part of a macro or not'''
		tokens = Macro.tokenize( line )
		if tokens and '#define' == tokens[ 0 ]:
			self.lines = []

		if self.lines is not None:
			self.lines.append( line )
			if tokens and tokens[ -1 ] == '\\':
				pass # more to come
			else:
				self.parseMacro( self.lines, self.lineNumber )
				self.lines = None

			return True
		else:
			return False


	def parse( self, line ):
		'''Primary method for learning macros.'''
		self.lineNumber += 1
		if self.isMacro( line ):
			pass


	def parseMacro( self, lines, number ):
		macro = Macro( lines, number )
		self.macros[ macro.identifier ] = macro
		#print macro
		#print macro.format( macro.args )


if __name__ == '__main__':
	with open( sys.argv[ 1 ], 'r' ) as handle:
		map( Translator().parse, handle )

