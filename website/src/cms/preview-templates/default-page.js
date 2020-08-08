import React from 'react';
import { DefaultPageTemplate } from '../../templates/default-page';
import CatchError from '../../core/catch-error';

function DefaultPagePreview({ entry }) {
	return (
		<CatchError>
			<DefaultPageTemplate {...entry.getIn(['data']).toJS()} isPreview={true} />
		</CatchError>
	);
}

export default DefaultPagePreview;
