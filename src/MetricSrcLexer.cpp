/*
   Copyright 2014 John Bailey

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "MetricSrcLexer.hpp"
#include "MetricUtils.hpp"
#include "clang/Lex/Preprocessor.h"
#include "clang/AST/ASTContext.h"

#include <iostream>

const static std::pair<std::string,MetricType_e> tokenToTypeMapData[] = {
	std::make_pair("bool", METRIC_TYPE_TOKEN_BOOL),
	std::make_pair("char", METRIC_TYPE_TOKEN_CHAR),
	std::make_pair("double", METRIC_TYPE_TOKEN_DOUBLE),
	std::make_pair("float", METRIC_TYPE_TOKEN_FLOAT),
	std::make_pair("int", METRIC_TYPE_TOKEN_INT),
	std::make_pair("long", METRIC_TYPE_TOKEN_LONG),
	std::make_pair("short", METRIC_TYPE_TOKEN_SHORT),
	std::make_pair("signed", METRIC_TYPE_TOKEN_SIGNED),
	std::make_pair("unsigned", METRIC_TYPE_TOKEN_UNSIGNED),
	std::make_pair("void", METRIC_TYPE_TOKEN_VOID),
	std::make_pair("const", METRIC_TYPE_TOKEN_CONST),
	std::make_pair("friend", METRIC_TYPE_TOKEN_FRIEND),
	std::make_pair("volatile", METRIC_TYPE_TOKEN_VOLATILE),
	std::make_pair("extern", METRIC_TYPE_TOKEN_EXTERN),
	std::make_pair("inline", METRIC_TYPE_TOKEN_INLINE),
	std::make_pair("register", METRIC_TYPE_TOKEN_REGISTER),
	std::make_pair("static", METRIC_TYPE_TOKEN_STATIC),
	std::make_pair("typedef", METRIC_TYPE_TOKEN_TYPEDEF),
	std::make_pair("virtual", METRIC_TYPE_TOKEN_VIRTUAL),
	std::make_pair("mutable", METRIC_TYPE_TOKEN_MUTABLE),
	std::make_pair("auto", METRIC_TYPE_TOKEN_AUTO),
	std::make_pair("asm", METRIC_TYPE_TOKEN_ASM),
	std::make_pair("break", METRIC_TYPE_TOKEN_BREAK),
	std::make_pair("case", METRIC_TYPE_TOKEN_CASE),
	std::make_pair("class", METRIC_TYPE_TOKEN_CLASS),
	std::make_pair("continue", METRIC_TYPE_TOKEN_CONTINUE),
	std::make_pair("default", METRIC_TYPE_TOKEN_DEFAULT),
	std::make_pair("delete", METRIC_TYPE_TOKEN_DELETE),
	std::make_pair("do", METRIC_TYPE_TOKEN_DO),
	std::make_pair("else", METRIC_TYPE_TOKEN_ELSE),
	std::make_pair("enum", METRIC_TYPE_TOKEN_ENUM),
	std::make_pair("for", METRIC_TYPE_TOKEN_FOR),
	std::make_pair("goto", METRIC_TYPE_TOKEN_GOTO),
	std::make_pair("if", METRIC_TYPE_TOKEN_IF),
	std::make_pair("new", METRIC_TYPE_TOKEN_NEW),
	std::make_pair("operator", METRIC_TYPE_TOKEN_OPERATOR),
	std::make_pair("private", METRIC_TYPE_TOKEN_PRIVATE),
	std::make_pair("protected", METRIC_TYPE_TOKEN_PROTECTED),
	std::make_pair("public", METRIC_TYPE_TOKEN_PUBLIC),
	std::make_pair("return", METRIC_TYPE_TOKEN_RETURN),
	std::make_pair("sizeof", METRIC_TYPE_TOKEN_SIZEOF),
	std::make_pair("struct", METRIC_TYPE_TOKEN_STRUCT),
	std::make_pair("switch", METRIC_TYPE_TOKEN_SWITCH),
	std::make_pair("this", METRIC_TYPE_TOKEN_THIS),
	std::make_pair("union", METRIC_TYPE_TOKEN_UNION),
	std::make_pair("while", METRIC_TYPE_TOKEN_WHILE),
	std::make_pair("namespace", METRIC_TYPE_TOKEN_NAMESPACE),
	std::make_pair("using", METRIC_TYPE_TOKEN_USING),
	std::make_pair("try", METRIC_TYPE_TOKEN_TRY),
	std::make_pair("catch", METRIC_TYPE_TOKEN_CATCH),
	std::make_pair("throw", METRIC_TYPE_TOKEN_THROW),
	std::make_pair("typeid", METRIC_TYPE_TOKEN_TYPEID),
	std::make_pair("template", METRIC_TYPE_TOKEN_TEMPLATE),
	std::make_pair("explicit", METRIC_TYPE_TOKEN_EXPLICIT),
	std::make_pair("true", METRIC_TYPE_TOKEN_TRUE),
	std::make_pair("false", METRIC_TYPE_TOKEN_FALSE),
	std::make_pair("typename", METRIC_TYPE_TOKEN_TYPENAME)
};

const std::map<std::string,MetricType_e> MetricSrcLexer::m_tokenToTypeMap( tokenToTypeMapData,
    tokenToTypeMapData + sizeof tokenToTypeMapData / sizeof tokenToTypeMapData[0]);

const static std::pair<clang::tok::TokenKind,MetricType_e> tokenKindToTypeMapData[] = {
	std::make_pair(clang::tok::exclaim, METRIC_TYPE_TOKEN_NOT),
	std::make_pair(clang::tok::exclaimequal, METRIC_TYPE_TOKEN_NOT),
	std::make_pair(clang::tok::percent, METRIC_TYPE_TOKEN_MODULO),
	std::make_pair(clang::tok::percentequal, METRIC_TYPE_TOKEN_MODULO_ASSIGN),
	std::make_pair(clang::tok::amp, METRIC_TYPE_TOKEN_AMP),
	std::make_pair(clang::tok::ampamp, METRIC_TYPE_TOKEN_AMPAMP),
	std::make_pair(clang::tok::pipepipe, METRIC_TYPE_TOKEN_PIPEPIPE),
	std::make_pair(clang::tok::ampequal, METRIC_TYPE_TOKEN_AND_ASSIGN),
	std::make_pair(clang::tok::l_paren, METRIC_TYPE_TOKEN_LPAREN),
	std::make_pair(clang::tok::r_paren, METRIC_TYPE_TOKEN_RPAREN),
	std::make_pair(clang::tok::star, METRIC_TYPE_TOKEN_ASTERISK),
	std::make_pair(clang::tok::starequal, METRIC_TYPE_TOKEN_ASTERISK_ASSIGN),
	std::make_pair(clang::tok::plus, METRIC_TYPE_TOKEN_PLUS),
	std::make_pair(clang::tok::plusplus, METRIC_TYPE_TOKEN_PLUSPLUS),
	std::make_pair(clang::tok::plusequal, METRIC_TYPE_TOKEN_PLUS_ASSIGN),
	std::make_pair(clang::tok::comma, METRIC_TYPE_TOKEN_COMMA),
	std::make_pair(clang::tok::minus, METRIC_TYPE_TOKEN_MINUS),
	std::make_pair(clang::tok::minusminus, METRIC_TYPE_TOKEN_MINUSMINUS),
	std::make_pair(clang::tok::minusequal, METRIC_TYPE_TOKEN_MINUS_ASSIGN),
	std::make_pair(clang::tok::arrow, METRIC_TYPE_TOKEN_MEMBER_POINTER),
	std::make_pair(clang::tok::period, METRIC_TYPE_TOKEN_MEMBER_REF),
	std::make_pair(clang::tok::ellipsis, METRIC_TYPE_TOKEN_ELLIPSIS),
	std::make_pair(clang::tok::slash, METRIC_TYPE_TOKEN_SLASH),
	std::make_pair(clang::tok::slashequal, METRIC_TYPE_TOKEN_SLASH_ASSIGN),
	std::make_pair(clang::tok::colon, METRIC_TYPE_TOKEN_COLON),
	std::make_pair(clang::tok::coloncolon, METRIC_TYPE_TOKEN_COLONCOLON),
	std::make_pair(clang::tok::less, METRIC_TYPE_TOKEN_LESS),
	std::make_pair(clang::tok::lessless, METRIC_TYPE_TOKEN_LESSLESS),
	std::make_pair(clang::tok::lesslessequal, METRIC_TYPE_TOKEN_LESSLESS_ASSIGN),
	std::make_pair(clang::tok::lessequal, METRIC_TYPE_TOKEN_LESS_EQUAL),
	std::make_pair(clang::tok::equal, METRIC_TYPE_TOKEN_ASSIGN),
	std::make_pair(clang::tok::equalequal, METRIC_TYPE_TOKEN_COMPARISON),
	std::make_pair(clang::tok::greater, METRIC_TYPE_TOKEN_MORE),
	std::make_pair(clang::tok::greaterequal, METRIC_TYPE_TOKEN_MORE_EQUAL),
	std::make_pair(clang::tok::greatergreater, METRIC_TYPE_TOKEN_MOREMORE),
	std::make_pair(clang::tok::greatergreaterequal, METRIC_TYPE_TOKEN_MOREMORE_ASSIGN),
	std::make_pair(clang::tok::question, METRIC_TYPE_TOKEN_QUESTION),
	std::make_pair(clang::tok::l_square, METRIC_TYPE_TOKEN_LSQUARE),
	std::make_pair(clang::tok::r_square, METRIC_TYPE_TOKEN_RSQUARE),
	std::make_pair(clang::tok::caret, METRIC_TYPE_TOKEN_CARET),
	std::make_pair(clang::tok::caretequal, METRIC_TYPE_TOKEN_CARET_ASSIGN),
	std::make_pair(clang::tok::l_brace, METRIC_TYPE_TOKEN_LBRACE),
	std::make_pair(clang::tok::r_brace, METRIC_TYPE_TOKEN_RBRACE),
	std::make_pair(clang::tok::pipe, METRIC_TYPE_TOKEN_PIPE),
	std::make_pair(clang::tok::pipeequal, METRIC_TYPE_TOKEN_PIPE_ASSIGN),
	std::make_pair(clang::tok::tilde, METRIC_TYPE_TOKEN_TILDE)

};

const std::map<clang::tok::TokenKind,MetricType_e> MetricSrcLexer::m_tokenKindToTypeMap( tokenKindToTypeMapData,
    tokenKindToTypeMapData + sizeof tokenKindToTypeMapData / sizeof tokenKindToTypeMapData[0]);


MetricSrcLexer::MetricSrcLexer(clang::CompilerInstance &p_CI, MetricUnit* p_topUnit, MetricOptions* p_options) : m_compilerInstance(p_CI), 
	                                                                                                             m_topUnit( p_topUnit ), 
	                                                                                                             m_options( p_options ),
																												 m_currentUnit( NULL )
{
}

MetricSrcLexer::~MetricSrcLexer(void)
{
}

void MetricSrcLexer::CountToken( clang::Token& p_token )
{		
	std::string tok_data;
	unsigned int tok_len = p_token.getLength();

	if( m_options->getDumpTokens() )
	{
		std::cout << "(" << p_token.getName();
	}

	switch( p_token.getKind() )
	{
		case clang::tok::raw_identifier:
			{
				tok_data = clang::StringRef(p_token.getRawIdentifierData(), tok_len).str();
				std::map<std::string,MetricType_e>::const_iterator typeLookup = m_tokenToTypeMap.find( tok_data );
				if( typeLookup != m_tokenToTypeMap.end() )
				{
					if( m_options->getDumpTokens() )
					{
						std::cout << ",reserved";
					}
					m_currentUnit->increment( (*typeLookup).second );
				}
				else
				{
					if( m_options->getDumpTokens() )
					{
						std::cout << ",unreserved";
					}
					if( m_currentUnit->isFnOrMethod() )
					{
						m_currentFnIdentifiers.insert( tok_data );
					}
					m_currentUnit->increment( METRIC_TYPE_TOKEN_UNRESERVED_IDENTIFIERS );
				}
			}
			break;
		case clang::tok::numeric_constant:
			tok_data = clang::StringRef(p_token.getLiteralData(), tok_len).str();
			if( m_currentUnit->isFnOrMethod() )
			{
				m_currentFnNumerics.insert( tok_data );
			}
			m_currentUnit->increment( METRIC_TYPE_TOKEN_NUMERIC_CONSTANTS );
			break;
		case clang::tok::string_literal:
			tok_data = clang::StringRef(p_token.getLiteralData(), tok_len).str();
			if( m_currentUnit->isFnOrMethod() )
			{
				m_currentFnStrings.insert( tok_data );
			}
			m_currentUnit->increment( METRIC_TYPE_TOKEN_STRING_LITERALS );
			break;
		default:
			{
				std::map<clang::tok::TokenKind,MetricType_e>::const_iterator typeLookup = m_tokenKindToTypeMap.find( p_token.getKind() );
				if( typeLookup != m_tokenKindToTypeMap.end() )
				{
					m_currentUnit->increment( (*typeLookup).second );
				}
				else
				{
					/* TODO */
				}
			}
			break;
	}

	if( m_options->getDumpTokens() )
	{
		if( tok_data.length() )
		{
			std::cout << "," << tok_data;
		}
		std::cout << ")";
	}
}

std::string MetricSrcLexer::FindFunction( clang::SourceManager& p_sm, clang::SourceLocation& p_loc, const SrcStartToFunctionMap_t* const p_fnMap )
{
	std::string ret_val = "";
	SrcStartToFunctionMap_t::const_iterator func_it = p_fnMap->begin();

	/* While we've not found a matching function and there are still functions to consider ... */
	while(( ret_val == "" ) && ( func_it != p_fnMap->end()))
	{
		/* Does the location we're considering match the function start or end or is it within those bounds? */
		if(( p_loc == (*func_it).first ) || 
		   ( p_loc == (*func_it).second.first ) ||
		   ( p_sm.isBeforeInTranslationUnit( (*func_it).first, p_loc ) &&
			 p_sm.isBeforeInTranslationUnit( p_loc, (*func_it).second.first )))
		{
			ret_val = (*func_it).second.second;
		}

		/* Next function in the map */
		func_it++;
	}

	return ret_val;
}

void MetricSrcLexer::CloseOutFnOrMtd( void )
{
	/* Have a current unit? */
	if( m_currentUnit != NULL )
	{
		MetricUnitType_e type = m_currentUnit->GetType();
		
		/* Is it of the appropriate type? */
		if( m_currentUnit->isFnOrMethod() )
		{
			/* Close off accumulated metrics */

			m_currentUnit->set( METRIC_TYPE_TOKEN_NUMERIC_CONSTANTS_UNIQ, m_currentFnNumerics.size() );
			m_currentFnNumerics.clear();
			m_currentUnit->set( METRIC_TYPE_TOKEN_STRING_LITERALS_UNIQ, m_currentFnStrings.size() );
			m_currentFnStrings.clear();
			m_currentUnit->set( METRIC_TYPE_TOKEN_UNRESERVED_IDENTIFIERS_UNIQ, m_currentFnIdentifiers.size() );
			m_currentFnIdentifiers.clear();
		}
	}
}

void MetricSrcLexer::LexSources( clang::SourceManager& p_sm, const SrcStartToFunctionMap_t* const p_fnMap )
{
	for( clang::SourceManager::fileinfo_iterator it = p_sm.fileinfo_begin();
		it != p_sm.fileinfo_end();
		it++ )
	{
		bool Invalid = false;
		clang::FileID fid = p_sm.translateFile( it->first );
		clang::StringRef Buffer = p_sm.getBufferData(fid, &Invalid);
		clang::Token result;
		std::string fileName = it->first->getName();

		if (Invalid)
		{
		   /* TODO: Something more useful */
			return;
		}
 
		if( SHOULD_INCLUDE_FILE( m_options, fileName ))
		{
			m_currentFileName = fileName;
			m_currentUnit = m_topUnit->getSubUnit(fileName, METRIC_UNIT_FILE);

			m_currentUnit->set( METRIC_TYPE_LINE_COUNT, countNewlines( Buffer ) );

			// Create a lexer starting at the beginning of this token.
			clang::Lexer TheLexer(p_sm.getLocForStartOfFile(fid), m_compilerInstance.getASTContext().getLangOpts(),
						   Buffer.begin(), Buffer.begin(), Buffer.end());

			TheLexer.SetCommentRetentionState(true);

			do
			{
				bool shouldLexToken = true;

				TheLexer.LexFromRawLexer(result);

				/* TODO: Could optimise this by not doing the function look-up for every single token, but 
				   determining whether or not the token's position has exceeded the range of the current function */
				std::string funcName = FindFunction( p_sm, result.getLocation(), p_fnMap );
				if( funcName != m_currentFunctionName )
				{
					if( m_options->getDumpTokens() )
					{
						std::cout << "[fn:" << funcName << "]";
					}
					CloseOutFnOrMtd();
				}
				if( funcName != "" ) 
				{
					if( SHOULD_INCLUDE_FUNCTION( m_options, funcName ))
					{
						m_currentUnit = m_topUnit->getSubUnit(fileName, METRIC_UNIT_FILE)->getSubUnit(funcName, METRIC_UNIT_FUNCTION);
					}
					else
					{
						shouldLexToken = false;
					}
				}
				else
				{
					m_currentUnit = m_topUnit->getSubUnit(fileName, METRIC_UNIT_FILE);
				}
				m_currentFunctionName = funcName;

				if( shouldLexToken )
				{
					CountToken( result );
				}

			} while (result.isNot(clang::tok::eof));

			CloseOutFnOrMtd();
		}
	} 
}