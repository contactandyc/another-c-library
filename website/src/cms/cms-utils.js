function safelyGetFmKey(pageContext, key) {
	if (safelyGetFrontMatter(pageContext)) {
		return safelyGetFrontMatter(pageContext)[key];
	} else {
		return null;
	}
}

function safelyGetFrontMatter(pageContext) {
	if (pageContext && pageContext.frontmatter) {
		return pageContext.frontmatter;
	} else {
		return {};
	}
}

function withFallback(value, fallback) {
	if (value) {
		return value;
	} else {
		return fallback;
	}
}

function safelyGetSiteConfig(page) {
	if (page && page.context) {
		return safelyGetFrontMatter(page.context);
	} else {
		return {};
	}
}

export {
	safelyGetFmKey,
	safelyGetFrontMatter,
	withFallback,
	safelyGetSiteConfig,
};
