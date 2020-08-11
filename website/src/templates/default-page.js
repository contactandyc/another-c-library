import React from 'react';
import { MDXProvider } from '@mdx-js/react';

import { CMS_COMPONENTS, CMS_SHORTCODES } from '../cms/cms-components';
import { safelyGetFrontMatter, withFallback } from '../cms/cms-utils';
import Heading from '../components/heading';
import SEO from '../components/seo';
import TableContents from '../components/table-contents';
import RenderMarkdown from '../core/render-markdown';

function DefaultPageTemplate({ location, title, body, children }) {
	const createLinkPair = (name, url) => {
		if (Array.isArray(name)) {
			name = name.join('');
		}

		const regex = /.*\/\/.*(\.|:)(\w*|\d*)/gi;
		let link = url.replace(regex, '');
		link = link.replace(/#.*/, '');
		link += `#${name.replace(/(\s|_|,)/g, '-').toLowerCase()}`;

		if (name.length > 27) {
			name = name.substring(0, 27);
			name += '...';
		}

		return [name, link];
	};

	let h2Links = [];

	if (children) {
		for (let i = 0; i < children.length; i++) {
			if (children[i].props.mdxType === 'h2') {
				h2Links.push(
					createLinkPair(
						children[i].props.children,
						location.href ? location.href : ''
					)
				);
			}
		}
	}

	return (
		<article className="DefaultPageTemplate">
			<SEO title={withFallback(title, '')} />
			<TableContents contents={h2Links} />
			<Heading tag={1}>{title}</Heading>
			{/* If children should be used instead of body, body will be empty, so it's safe to have both */}
			<RenderMarkdown md={body} />
			{/* Include children to support any normal MDX files in the project */}
			<MDXProvider components={{ ...CMS_COMPONENTS, ...CMS_SHORTCODES }}>
				{children}
			</MDXProvider>
		</article>
	);
}

function DefaultPage({ pageContext, ...props }) {
	return (
		<DefaultPageTemplate
			{...safelyGetFrontMatter(pageContext)}
			{...props}
			isPreview={false}
		/>
	);
}

export { DefaultPage, DefaultPageTemplate };
