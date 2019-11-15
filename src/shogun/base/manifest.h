/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Sergey Lisitsyn, Sanuj Sharma, Viktor Gal
 */

#ifndef _MANIFEST_H_
#define _MANIFEST_H_

#include <shogun/base/metaclass.h>

#include <unordered_set>

namespace shogun
{

	class Library;

	/** @brief Manifest stores meta-data of Library.
	 * Each manifest has description and a set of meta-classes
	 * (see @ref MetaClass) which are responsible for
	 * creating instances of exported classes.
	 */
	class Manifest
	{
	public:
		/** Constructor to initialize hash from name
		 * @param description description for the Library
		 * @param metaclasses list of meta-classes for exported classes
		 */
		Manifest(const std::string& description,
			const std::initializer_list<std::pair<std::string,Any>> metaclasses);

		/** Copy constructor
		 * @param other Manifest object to be copied
		 */
		Manifest(const Manifest& other);

		/** Move constructor
		 * @param other Manifest object to be copied
		 */
		Manifest(Manifest&& other);

		/** Class Assignment operator
		 * @param other manifest object to be assigned
		 */
		Manifest& operator=(const Manifest& other);

		/** Equality operator
		 * @param first first Manifest
		 * @param second second Manifest
		 */
		friend bool operator==(const Manifest& first, const Manifest& second);

		/** Inequality operator
		 * @param first first Manifest
		 * @param second second Manifest
		 */
		friend bool operator!=(const Manifest& first, const Manifest& second);

		/** Destructor */
		~Manifest();

		/** Returns meta-class by its name.
		 *
		 * @param name name of meta-class to obtain
		 * @return object of meta-class
		 */
		template <typename T>
		MetaClass<T> class_by_name(const std::string& name) const
		{
			auto clazz = find_class(name);
			if constexpr(std::is_same_v<SGObject, T>)
				return clazz.cast<MetaClass<T>>();
			return any_cast<MetaClass<T>>(clazz);
		}

		/** @return description stored in the manifest. */
		std::string description() const;

		/** @return list of classes in this manifest. */
		std::unordered_set<std::string> class_list() const;

	protected:
		/** Adds mapping from class name to MetaClass object of
		 * class (stored as Any object) corresponding to the name.
		 * The map is stored in Self.
		 * @param name name for class
		 * @param clazz class
		 */
		void add_class(const std::string& name, Any clazz);

		/** Finds MetaClass object (stored as Any object) of class
		 * corresponding to the input name.
		 * @param name name for class
		 * @return
		 */
		Any find_class(const std::string& name) const;

	private:
		class Self;
		std::unique_ptr<Self> self;
	};

/** Starts manifest declaration with its description.
 * Always immediately follow this macro with
 * @ref EXPORT or @ref END_MANIFEST.
 */
#define BEGIN_MANIFEST(DESCRIPTION)                             \
extern "C" Manifest shogunManifest()                            \
{                                                               \
	static Manifest manifest(DESCRIPTION,{                      \

/** Declares class to be exported.
 * Always use this macro between @ref BEGIN_MANIFEST and
 * @ref END_MANIFEST
 */
#define EXPORT(CLASSNAME, BASE_CLASSNAME, IDENTIFIER)                           \
	std::make_pair(IDENTIFIER"_sgo", make_any(                                        \
		MetaClass<SGObject>(make_any<std::shared_ptr<SGObject>>(    \
				[]() -> std::shared_ptr<SGObject>                         \
				{                                                               \
					return std::make_shared<CLASSNAME>();                       \
				}                                                               \
				)))),                                                           \
	std::make_pair(IDENTIFIER, make_any(                                        \
		MetaClass<BASE_CLASSNAME>(make_any<std::shared_ptr<BASE_CLASSNAME>>(    \
				[]() -> std::shared_ptr<BASE_CLASSNAME>                         \
				{                                                               \
					return std::make_shared<CLASSNAME>();                       \
				}                                                               \
				)))),                                                           \

/** Ends manifest declaration.
 * Always use this macro after @ref BEGIN_MANIFEST
 */
#define END_MANIFEST()                                          \
		});                                                     \
	return manifest;                                            \
}

}

#endif //_MANIFEST_H_
