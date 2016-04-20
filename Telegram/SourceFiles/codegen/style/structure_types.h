/*
This file is part of Telegram Desktop,
the official desktop version of Telegram messaging app, see https://telegram.org

Telegram Desktop is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

It is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

In addition, as a special exception, the copyright holders give permission
to link the code of portions of this program with the OpenSSL library.

Full license: https://github.com/telegramdesktop/tdesktop/blob/master/LICENSE
Copyright (c) 2014-2016 John Preston, https://desktop.telegram.org
*/
#pragma once

#include <memory>
#include <QtCore/QString>
#include <QtCore/QStringList>

namespace codegen {
namespace style {
namespace structure {

// List of names, like overview.document.bg
using FullName = QStringList;
inline std::string logFullName(const FullName &name) {
	return name.join('.').toStdString();
}

struct Variable;

enum class TypeTag {
	Invalid,
	Int,
	Double,
	Pixels,
	String,
	Color,
	Point,
	Sprite,
	Size,
	Transition,
	Cursor,
	Align,
	Margins,
	Font,
	Struct,
};

struct Type {
	TypeTag tag;
	FullName name; // only for type == ClassType::Struct

	explicit operator bool() const {
		return (tag != TypeTag::Invalid);
	}
};
inline bool operator==(const Type &a, const Type &b) {
	return (a.tag == b.tag) && (a.name == b.name);
}
inline bool operator!=(const Type &a, const Type &b) {
	return !(a == b);
}

namespace data {

inline int pxAdjust(int value, int scale) {
	return qRound((value * scale / 4.) + (value > 0 ? -0.01 : 0.01));
}

struct point {
	int x, y;
};
struct sprite {
	int left, top, width, height;
};
struct size {
	int width, height;
};
struct color {
	uchar red, green, blue, alpha;
};
struct margins {
	int left, top, right, bottom;
};
struct font {
	enum Flag {
		Bold      = 0x01,
		Italic    = 0x02,
		Underline = 0x04,
	};
	std::string family;
	int size;
	int flags;
};
struct field; // defined after Variable is defined
using fields = QList<field>;

} // namespace data

class Value {
public:
	Value();
	Value(data::point value);
	Value(data::sprite value);
	Value(data::size value);
	Value(data::color value);
	Value(data::margins value);
	Value(data::font value);
	Value(const FullName &type, data::fields value);

	// Can be only double.
	Value(TypeTag type, double value);

	// Can be int / pixels.
	Value(TypeTag type, int value);

	// Can be string / transition / cursor / align.
	Value(TypeTag type, std::string value);

	// Default constructed value (uninitialized).
	Value(Type type, Qt::Initialization);

	Type type() const { return type_; }
	int Int() const { return data_->Int(); }
	double Double() const { return data_->Double(); }
	std::string String() const { return data_->String(); }
	data::point Point() const { return data_->Point(); }
	data::sprite Sprite() const { return data_->Sprite(); };
	data::size Size() const { return data_->Size(); };
	data::color Color() const { return data_->Color(); };
	data::margins Margins() const { return data_->Margins(); };
	data::font Font() const { return data_->Font(); };
	const data::fields *Fields() const { return data_->Fields(); };
	data::fields *Fields() { return data_->Fields(); };

	explicit operator bool() const {
		return type_.tag != TypeTag::Invalid;
	}

	Value makeCopy(const FullName &copyOf) const {
		Value result(*this);
		result.copyOf_ = copyOf;
		return result;
	}

	const FullName &copyOf() const {
		return copyOf_;
	}

private:
	class DataBase {
	public:
		virtual int Int() const { return 0; }
		virtual double Double() const { return 0.; }
		virtual std::string String() const { return std::string(); }
		virtual data::point Point() const { return {}; };
		virtual data::sprite Sprite() const { return {}; };
		virtual data::size Size() const { return {}; };
		virtual data::color Color() const { return {}; };
		virtual data::margins Margins() const { return {}; };
		virtual data::font Font() const { return {}; };
		virtual const data::fields *Fields() const { return nullptr; };
		virtual data::fields *Fields() { return nullptr; };
		virtual ~DataBase() {
		}
	};
	struct DataTypes;

	Value(TypeTag type, std::shared_ptr<DataBase> &&data);

	Type type_;
	std::shared_ptr<DataBase> data_;

	FullName copyOf_; // for copies of existing named values

};

struct Variable {
	FullName name;
	Value value;

	explicit operator bool() const {
		return !name.isEmpty();
	}
};

namespace data {
struct field {
	enum class Status {
		Uninitialized,
		Implicit,
		Explicit
	};
	Variable variable;
	Status status;
};
} // namespace data

struct StructField {
	FullName name;
	Type type;

	explicit operator bool() const {
		return !name.isEmpty();
	}
};

struct Struct {
	FullName name;
	QList<StructField> fields;

	explicit operator bool() const {
		return !name.isEmpty();
	}
};

} // namespace structure
} // namespace style
} // namespace codegen