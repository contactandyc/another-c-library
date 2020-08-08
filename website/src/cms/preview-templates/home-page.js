import React from 'react';
import { HomePageTemplate } from '../../templates/home-page';
import CatchError from '../../core/catch-error';

function HomePagePreview({ entry }) {
	return (
		<CatchError>
			<HomePageTemplate {...entry.getIn(['data']).toJS()} isPreview={true} />
		</CatchError>
	);
}

export default HomePagePreview;
