/*
 * This addon adds Tiled map support to the Allegro game library.
 * Copyright (c) 2012 Damien Radtke - www.damienradtke.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * For more information, visit http://www.gnu.org/copyleft
 *
 *                               ---
 *
 * Helper methods for parsing XML with libxml2.
 */

#include "xml.h"

/*
 * Given a parent XML node and the name of all desired children,
 * returns a list of all children with that name
 */
GSList *get_children_for_name(xmlNode *parent, char *name)
{
	GSList *list = NULL;
	
	if (!parent->children)
		return list;

	xmlNode *child = parent->children->next;
	while (child) {
		if (!strcmp((const char*)child->name, name)) {
			list = g_slist_prepend(list, child);
		}

		child = child->next;
	}

	return list;
}

/*
 * Alternate version that allows two possible names to be specified.
 * Tile/object layers have different node names ("layer" vs "objectgroup"),
 * but their order needs to be preserved. This method is a workaround
 * for that.
 */
GSList *get_children_for_either_name(xmlNode *parent, char *name1, char *name2)
{
	GSList *list = NULL;
	
	if (!parent->children)
		return list;

	xmlNode *child = parent->children->next;
	while (child) {
		if (!strcmp((const char*)child->name, name1) || !strcmp((const char*)child->name, name2)) {
			list = g_slist_prepend(list, child);
		}

		child = child->next;
	}

	return list;
}

/*
 * Given a parent XML node and the name of the desired child,
 * locates and returns the first child with that name
 */
xmlNode *get_first_child_for_name(xmlNode *parent, char *name)
{
	if (!parent->children)
		return NULL;

	xmlNode *child = parent->children->next;

	while  (child != NULL) {
		if (!strcmp((const char*)child->name, name)) {
			return child;
		} else {
			child = child->next;
		}
	}

	return NULL;
}

/*
 * Gets the value of an attribute from a node
 * The returned value must be copied if it will remain a string
 */
char *get_xml_attribute(xmlNode *node, char *name)
{
	xmlAttr *attrs = node->properties;

	while (attrs != NULL) {
		if (!strcmp((const char*)attrs->name, name))
			return (char *)attrs->children->content;

		attrs = attrs->next;
	}

	return NULL;
}

/*
 * Gets the value of an attribute from a node
 * The returned value must be copied if it will remain a string
 */
int *get_xml_attribute_int(xmlNode *node, char *name)
{
	xmlAttr *attrs = node->properties;

	while (attrs != NULL) {
		if (!strcmp((const char*)attrs->name, name))
			return (int *)attrs->children->content;

		attrs = attrs->next;
	}

	return NULL;
}
