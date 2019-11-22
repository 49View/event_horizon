#pragma once

#include <memory>

class ServiceFactory {
public:
	template<typename T>
	static std::shared_ptr<T> get() {
		static std::shared_ptr<T> instance = std::make_shared<T>();
		return instance;
	}

	template<typename T>
	static std::shared_ptr<T> create() {
		return std::make_shared<T>();
	}

	template<typename T, typename Builder>
	static std::shared_ptr<T> create( const Builder& b ) {
		return std::make_shared<T>(b);
	}

	template<typename T, typename Builder>
	static std::shared_ptr<T> create( Builder& b ) {
		return std::make_shared<T>(b);
	}

	template<typename T, typename ... Args>
	static std::shared_ptr<T> handler( Args&& ... args ) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	static std::unique_ptr<T> createUnique() {
		return std::make_unique<T>();
	}

	template<typename T>
	static std::shared_ptr<T> create( const std::string& _name ) {
		return std::make_shared<T>( _name );
	}

	template<typename T>
	static std::shared_ptr<T> clone(T& _source) {
		return std::make_shared<T>(_source);
	}
};

