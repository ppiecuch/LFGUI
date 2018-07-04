#ifndef XTNSTREAMBUF_H
#define XTNSTREAMBUF_H

#ifdef QT_CORE_LIB

#include <ios>
#include <stdexcept>
#include <fstream>

#include <QIODevice>
#include <QFile>

class QStdStreamBuf : public std::streambuf
{
public:
	QStdStreamBuf(QIODevice *dev) : std::streambuf(), m_dev(dev)
	{
		// Initialize get pointer.  This should be zero so that underflow is called upon first read.
		this->setg(0, 0, 0);
	}

protected:
	virtual std::streamsize xsgetn(std::streambuf::char_type *str, std::streamsize n)
	{
		return m_dev->read(str, n);
	}

	virtual std::streamsize xsputn(const std::streambuf::char_type *str, std::streamsize n)
	{
		return m_dev->write(str, n);
	}

	virtual std::streambuf::pos_type seekoff(std::streambuf::off_type off, std::ios_base::seekdir dir, std::ios_base::openmode /*__mode*/)
	{
		switch(dir)
		{
		case std::ios_base::beg:
			break;
		case std::ios_base::end:
			off = m_dev->size() - off;
			break;
		case std::ios_base::cur:
			off = m_dev->pos() + off;
			break;
		default:
			throw std::runtime_error("Operation not implemented");
		}
		if(m_dev->seek(off))
			return m_dev->pos();
		else
			return std::streambuf::pos_type(std::streambuf::off_type(-1));
	}
	virtual std::streambuf::pos_type seekpos(std::streambuf::pos_type off, std::ios_base::openmode /*__mode*/)
	{
		if(m_dev->seek(off))
			return m_dev->pos();
		else
			return std::streambuf::pos_type(std::streambuf::off_type(-1));
	}

	virtual std::streambuf::int_type underflow()
	{
		// Read enough bytes to fill the buffer.
		std::streamsize len = sgetn(m_inbuf, sizeof(m_inbuf)/sizeof(m_inbuf[0]));

		// Since the input buffer content is now valid (or is new)
		// the get pointer should be initialized (or reset).
		setg(m_inbuf, m_inbuf, m_inbuf + len);

		// If nothing was read, then the end is here.
		if(len == 0)
			return traits_type::eof();

		// Return the first character.
		return traits_type::not_eof(m_inbuf[0]);
	}


private:
	static const std::streamsize BUFFER_SIZE = 1024;
	std::streambuf::char_type m_inbuf[BUFFER_SIZE];
	QIODevice *m_dev;
};

class QStdIStream : public std::istream
{
public:
	QStdIStream(QIODevice* dev) : std::istream(m_buf = new QStdStreamBuf(dev)) {}
	virtual ~QStdIStream()
	{
		std::istream::rdbuf(0);
		delete m_buf;
	}

private:
	QStdStreamBuf * m_buf;
};

class QStdFStream : public std::fstream
{
public:
	QStdFStream() : std::fstream() {
        std::istream::rdbuf( m_buf = new QStdStreamBuf(m_file = new QFile) );
    }

	QStdFStream(const std::string filename,  ios_base::openmode mode = ios_base::in) : std::fstream() {
        std::istream::rdbuf( m_buf = new QStdStreamBuf(m_file = new QFile) );
        open(filename.c_str(), mode);
    }

	void open (const char* filename,  ios_base::openmode mode = ios_base::in)
    {
        QIODevice::OpenMode flags = 0;
        if (mode & ios_base::in) {
            flags = QIODevice::ReadOnly;
        } else if (mode & ios_base::out) {
            flags = QIODevice::WriteOnly;
        }
        if (mode & ios_base::app) {
            flags |= QIODevice::Append;
        } else if (mode & ios_base::trunc) {
            flags |= QIODevice::Truncate;
        }

        m_file->setFileName(filename);
        if(!m_file->open(flags))
            if(QFile::exists(QString(":%1").arg(filename))) {
                m_file->setFileName(QString(":%1").arg(filename));
                m_file->open(flags);
            }

        if(!m_file->isOpen())
            setstate(std::ios::failbit);
        else if (mode & ios_base::ate)
            m_file->seek(m_file->size());
    }

    bool is_open() const { return m_file->isOpen(); }

	virtual ~QStdFStream()
	{
		std::istream::rdbuf(0);
		delete m_buf;
        delete m_file;
	}

private:
    QFile * m_file;
	QStdStreamBuf * m_buf;
};

namespace std { namespace xtn {
    typedef QStdFStream ifstream;
}}

#else

#include <iostream>

namespace std { namespace xtn {
    typedef std::ifstream ifstream;
}

#endif


#endif // XTNSTREAMBUF_H
