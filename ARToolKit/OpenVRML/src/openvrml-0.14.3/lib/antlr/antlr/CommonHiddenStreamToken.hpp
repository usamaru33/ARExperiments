#ifndef INC_CommonHiddenStreamToken_hpp__
#define INC_CommonHiddenStreamToken_hpp__

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/RIGHTS.html
 *
 * $Id: CommonHiddenStreamToken.hpp,v 1.1.1.1 2003/04/06 22:26:27 braden Exp $
 */

#include <antlr/config.hpp>
#include <antlr/CommonToken.hpp>

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

class ANTLR_API CommonHiddenStreamToken : public CommonToken {
protected:
	RefToken hiddenBefore;
	RefToken hiddenAfter;

public:
	CommonHiddenStreamToken();
	CommonHiddenStreamToken(int t, const ANTLR_USE_NAMESPACE(std)string& txt);
	CommonHiddenStreamToken(const ANTLR_USE_NAMESPACE(std)string& s);

	RefToken getHiddenAfter();
	RefToken getHiddenBefore();

   static RefToken factory();

	void setHiddenAfter(RefToken t);
	void setHiddenBefore(RefToken t);
};

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

#endif //INC_CommonHiddenStreamToken_hpp__
