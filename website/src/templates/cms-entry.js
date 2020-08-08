import React from 'react';

import { HomePage } from './home-page';
import { DefaultPage } from './default-page';
import NotFoundPage from './not-found';
import AppLayout from './app-layout';
import { safelyGetFrontMatter } from '../cms/cms-utils';

function CMSTemplate(props) {
	// Extend this template map to allow your users to choose a page layout from the CMS
	// If you're only looking for how to specify a different template per content folder, see:
	// https://www.gatsbyjs.org/packages/gatsby-mdx/#installation
	const componentTemplateMap = {
		'home-page': HomePage,
		'hidden-page': NotFoundPage,
	};

	const { pageContext } = props;
	const { templateKey } = safelyGetFrontMatter(pageContext);
	const Page = componentTemplateMap[templateKey];

	return (
		<AppLayout>
			{Page ? <Page {...props} /> : <DefaultPage {...props} />}
		</AppLayout>
	);
}

export default CMSTemplate;
