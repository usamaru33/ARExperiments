#ifndef INC_InputBuffer_hpp__
#define INC_InputBuffer_hpp__

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/RIGHTS.html
 *
 * $Id: InputBuffer.hpp,v 1.1.1.1 2003/04/06 22:26:27 braden Exp $
 */

#include <antlr/config.hpp>
#include <antlr/CircularQueue.hpp>
#include <string>

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

/** A Stream of characters fed to the lexer from a InputStream that can
 * be rewound via mark()/rewind() methods.
 * <p>
 * A dynamic array is used to buffer up all the input characters.  Normally,
 * "k" characters are stored in the buffer.  More characters may be stored during
 * guess mode (testing syntactic predicate), or when LT(i>k) is referenced.
 * Consumption of characters is deferred.  In other words, reading the next
 * character is not done by conume(), but deferred until needed by LA or LT.
 * <p>
 *
 * @see antlr.CharQueue
 */
class ANTLR_API InputBuffer {
public:
	/** Create a character buffer */
	InputBuffer()
	: nMarkers(0), markerOffset(0), numToConsume(0)
	{
	}

	virtual ~InputBuffer()
	{
	}

	/// Reset the input buffer to empty state
	virtual inline void reset( void )
	{
		nMarkers = 0;
		markerOffset = 0;
		numToConsume = 0;
		queue.clear();
	}

	/** This method updates the state of the input buffer so that
	 * the text matched since the most recent mark() is no longer
	 * held by the buffer.  So, you either do a mark/rewind for
	 * failed predicate or mark/commit to keep on parsing without
	 * rewinding the input.
	 */
	inline void commit( void )
	{
		nMarkers--;
	}

	/** Mark another character for deferred consumption */
	virtual inline void consume()
	{
		numToConsume++;
	}

	/** Ensure that the character buffer is sufficiently full */
	virtual void fill(int amount);

	/** Override this in subclasses to get the next character */
	virtual int getChar()=0;

	/** Get a lookahead character */
	virtual inline int LA(int i)
	{
		fill(i);
		return queue.elementAt(markerOffset + i - 1);
	}

	/** Return an integer marker that can be used to rewind the buffer to
	 * its current state.
	 */
	virtual int mark();
	/// Are there any marks active in the InputBuffer
	virtual inline bool isMarked() const
	{
		return (nMarkers != 0);
	}
	/** Rewind the character buffer to a marker.
	 * @param mark Marker returned previously from mark()
	 */
	virtual void rewind(int mark);

	ANTLR_USE_NAMESPACE(std)string getLAChars() const;

	ANTLR_USE_NAMESPACE(std)string getMarkedChars() const;

protected:
	// char source
	// leave to subclasses

	// Number of active markers
	int nMarkers; // = 0;

	// Additional offset used when markers are active
	int markerOffset; // = 0;

	// Number of calls to consume() since last LA() or LT() call
	int numToConsume; // = 0;

	// Circular queue
	CircularQueue<int> queue;

	/** Sync up deferred consumption */
	void syncConsume();

private:
	InputBuffer(const InputBuffer& other);
	InputBuffer& operator=(const InputBuffer& other);
};

/** Sync up deferred consumption */
inline void InputBuffer::syncConsume() {
	if (numToConsume > 0)
	{
		if (nMarkers > 0)
			markerOffset += numToConsume;
		else
			queue.removeItems( numToConsume );
		numToConsume = 0;
	}
}

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

#endif //INC_InputBuffer_hpp__
