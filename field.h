/*
 * General data container
 */


#ifndef UTIL__FIELD_H_
#define UTIL__FIELD_H_

class AnyField
{
public:
	virtual ~AnyField() = 0;
};

template <typename T>
class Field : public AnyField
{
protected:
	T m_data_;

public:
	static Field<T>* Of(const T& data) { return new Field<T>(data); }

	explicit Field(const T& data) : m_data_(data) {}

	T& Get() { return m_data_; }
	void Set(T& data) { m_data_ = data; }

	T Get() const { return m_data_; }

	~Field() override {};
};

#endif // UTIL__FIELD_H_
