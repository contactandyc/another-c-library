import CMS from 'netlify-cms-app';

import HomePagePreview from './preview-templates/home-page';
import DefaultPagePreview from './preview-templates/default-page';

// Not reliably loaded by registerPreviewStyle, so import directly
import '../app.css';

// Add Previews
CMS.registerPreviewTemplate('home', HomePagePreview);
CMS.registerPreviewTemplate('docs', DefaultPagePreview);
