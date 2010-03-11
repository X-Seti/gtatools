/*
 * TXDArchive.h
 *
 *  Created on: 08.01.2010
 *      Author: alemariusnexus
 */

#ifndef TXDARCHIVE_H_
#define TXDARCHIVE_H_

#include "../gf_config.h"
#include "../gta.h"
#include "../gf_filetype.h"
#include "TXDException.h"
#include "TXDTexture.h"
#include "TXDVisitor.h"
#include <istream>
#include <string>
#include <algorithm>
#include <cctype>
#include <cstdio>

using std::istream;
using std::string;

#define TXD_COMPRESSION_NONE 0
#define TXD_COMPRESSION_DXT1 1
#define TXD_COMPRESSION_DXT3 3

class TXDVisitor;



/**	\brief A loader class for GTA TXD files.
 *
 *	The class is designed as a streaming loader, so it can't be used for caching. Some methods for random
 *	access are included, though. It can be used for streams without random acces (without support for tellg()
 *	and seekg()). Note that without random acces, some methods might not be available.
 */
class TXDArchive {
public:
	static bool isValidFilename(const std::string& filename) {
		return GFGuessFileType(filename) == GF_TYPE_TXD;
	}

public:
	/**	\brief Constructs a TXDArchive from the given stream.
	 *
	 * 	This reads/skips the header of the TXD file to the first TextureNative section.
	 *
	 *	@param stream The stream.
	 */
	TXDArchive(istream* stream);

	/**	\brief Deletes this TXDArchive.
	 *
	 * 	Note that the TXDTextures will NOT be automatically deleted to allow using them standalone.
	 */
	~TXDArchive();

	/**	\brief Reads a TXDTexture without data from the stream.
	 *
	 * 	This parses the TextureNative section header and sets the stream in front of the texture data. The
	 *	returned TXDTexture must be deleted when not used anymore.
	 *
	 *	@return The TXDTexture that was read.
	 */
	TXDTexture* nextTexture();

	/**	\brief Reads raw texture data from the stream.
	 *
	 * 	This method reads in an array of bytes which is the data of the texture. Note that this is raw
	 * 	information, so it may be compressed and/or paletted and can have different formats. Use
	 * 	TXDTexture::convert() to bring it to a more suitable form for processing.
	 * 	The stream position must be right in front of the data. The TXDTexture parameter is used only to
	 * 	determine the format of the data which will follow.
	 *
	 * 	As this method may produce a buffer overflow, you should only use this function if you really know
	 * 	what you do. Always try to use readTextureData(TXDTexture*) instead.
	 *
	 *	@param dest Where to write the destination data.
	 *	@param texture The texture whose data will be read.
	 *	@see readTextureData(TXDTexture*)
	 */
	void readTextureData(uint8_t* dest, TXDTexture* texture);

	/**	\brief Reads raw texture data from the stream into a new array.
	 *
	 * 	This does the same as readTextureData(uint8_t*, TXDTexture*), but it creates a new array which will
	 * 	always be of exactly the right size to hold the data. You are responsible for deleting it when not
	 * 	needed anymore.
	 *
	 *	@param texture The texture whose data will be read.
	 *	@return The raw texture data.
	 *	@see readTextureData(uint8_t*, TXDTexture*)
	 */
	uint8_t* readTextureData(TXDTexture* texture);

	/**	\brief Repositions the stream to the position in front of the data of the given texture.
	 *
	 * 	This can be used to return to an already read texture to retrieve it's data. Note that seekg()
	 * 	must be a valid operation on the stream in order for this method to work. You can't use it for
	 * 	streams without random-access.
	 *
	 *	@param texture The texture to go to.
	 */
	void gotoTexture(TXDTexture* texture);

	/**	\brief Visits the given visitor with the given texture.
	 *
	 * 	This calls TXDVisitor::readHeader() with the texture. If that call returns true, it calls
	 * 	TXDVisitor::readEntry() afterwards.
	 *
	 *	@param visitor
	 *	@param texture
	 *	@see TXDVisitor
	 *	@see visitAll()
	 */
	void visit(TXDVisitor* visitor, TXDTexture* texture);

	/**	\brief Visits the given visitor with every texture inside the TXD archive.
	 *
	 * 	This calls visit() for every entry inside the archive. All textures, even those which were already
	 * 	read (by using gotoTexture() for these ones), will be used.
	 *
	 *	@param visitor The visitor.
	 *	@see visit()
	 *	@see TXDVisitor
	 */
	void visitAll(TXDVisitor* visitor);

	/**	\brief Returns the number of textures inside the archive.
	 *
	 *	@return The number of textures.
	 */
	int16_t getTextureCount() { return textureCount; }

private:
	/**	\brief Reads an RW section header with a given ID.
	 *
	 * 	This uses RwReadSectionHeader() to read the header. Also, it checks whether the ID read matches
	 * 	the ID given as parameter. If it doesn't, it throws an exception.
	 *
	 *	@param stream The stream to read from.
	 *	@param header Where to header should be read to.
	 *	@param id The ID which is assumed to be the one of the header.
	 */
	void readSectionHeaderWithID(istream* stream, RwSectionHeader& header, uint32_t id);

private:
	istream* stream;
	istream::streamoff bytesRead;
	TXDTexture** indexedTextures;
	istream::streamoff* textureNativeStarts;
	int16_t readIndex;
	int16_t textureCount;
	TXDTexture* currentTexture;
	int32_t currentTextureNativeSize;
	istream::streamoff currentTextureNativeStart;
};

#endif /* TXDARCHIVE_H_ */