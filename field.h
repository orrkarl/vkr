#ifndef UTIL__FIELD_H_
#define UTIL__FIELD_H_

class AnyField
{
public:
	virtual ~AnyField() = default;
};

template <typename T>
class Field : public AnyField
{
private:
	T data_;

public:
	template <typename G>
	static Field<G>* Of(const G& data) { return new Field<G>(data); }

	explicit Field(const T& data) : data_(data) {}

	T& Get() { return data_; }
	void Set(T& data) { data_ = data; }

	T Get() const { return data_; }
};

#endif // UTIL__FIELD_H_
