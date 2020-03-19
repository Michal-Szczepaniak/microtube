/**
 * Copyright (c) 2017-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

const React = require('react');

const CompLibrary = require('../../core/CompLibrary.js');
const MarkdownBlock = CompLibrary.MarkdownBlock; /* Used to read markdown */
const Container = CompLibrary.Container;
const GridBlock = CompLibrary.GridBlock;

const siteConfig = require(process.cwd() + '/siteConfig.js');

class Button extends React.Component {
  render() {
    return (
      <div className="pluginWrapper buttonWrapper" id={this.props.key}>
        <a className="button" href={this.props.href} target={this.props.target}>
          {this.props.children}
        </a>
      </div>
    );
  }
}

Button.defaultProps = {
  target: '_self',
};

class HomeSplash extends React.Component {
  render() {
    return (
      <div className="homeContainer">
        <div className="homeSplashFade">
          <div className="wrapper homeWrapper">
            <div className="projectLogo">
              <img src={siteConfig.baseUrl + 'img/QEasyDownloader.png'} />
            </div>
            <div className="inner">
              <h2 className="projectTitle">
                {siteConfig.title}
                <small>{siteConfig.tagline}</small>
              </h2>
              <div className="section promoSection">
                <div className="promoRow">
                  <div className="pluginRowBlock">
                    <Button
                      href={
                        siteConfig.baseUrl +
                        'docs/' +
                        'Installation.html'
                      }>
                      Get Started
                    </Button>
                    <Button
                      href={
                        siteConfig.baseUrl +
                        'docs/' +
                        'QEasyDownloaderErrorCodes.html'
                      }>
                      API Reference
                    </Button>
	            <Button href={'https://github.com/antony-jr/QEasyDownloader'}>
	    	      View on Github
	    	    </Button>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    );
  }
}

class Index extends React.Component {
  render() {
    let language = this.props.language || 'en';
    const showcase = siteConfig.users
      .filter(user => {
        return user.pinned;
      })
      .map(user => {
        return (
          <a href={user.infoLink}>
            <img src={user.image} title={user.caption} />
          </a>
        );
      });

    return (
      <div>
        <HomeSplash language={language} />
        <div className="mainContainer">
          <Container padding={['bottom', 'top']}>
            <GridBlock
              align="center"
              contents={[
                {
                  content: 'QEasyDownloader fully supports Qt Projects by flowing with its event loop and'+
			   ' thus you will have no problem integrating QEasyDownloader with your Qt Project.',
                  image: siteConfig.baseUrl + 'img/qt.png',
                  imageAlign: 'top',
                  title: 'Supports Qt\'s Event Loop.',
                },
		{
                  content: 'QEasyDownloader can automatically resume downloads on incomplete files , It also '+
                           'allows the user to pause or resume the download any time! Thus giving the user ' +
			   'a best downloading experience.',
                  image: siteConfig.baseUrl + 'img/pause.png',
                  imageAlign: 'top',
                  title: 'Pause , Resume and Partial Retrive with ease!',
                },
		{
                  content: 'QEasyDownloader supports queueing and thus it can download large number of files '+
                           'with ease , this makes your day even more easier!',
                  image: siteConfig.baseUrl + 'img/queue.png',
                  imageAlign: 'top',
                  title: 'Support for Queueing of Download\'s.',
                },
		{
                  content: 'QEasyDownloader does not like leaving heavy traces for the users and thus '+
                           'it is packed with all its amazing features in a small C++ library , this ' +
			   'makes QEasyDownloader easier to integrate with your project.',
                  image: siteConfig.baseUrl + 'img/light.png',
                  imageAlign: 'top',
                  title: 'Very Light-Weight!',
                },
		{
                  content: '<center>This project is rated by <b>Codacy</b> with a <b>A Project Certification</b><br>' +
                           ' and thus integrating this header will not affect your source , So this project is also<br>'+
                           ' best suited for those who care about code taste.<br></center>',
                  image: siteConfig.baseUrl + 'img/clean_code.png',
                  imageAlign: 'top',
                  title: '<center>Clean C++ API.</center>',
                },

              ]}
              layout="fourColumn"
            />
          </Container>

          <Container padding={['bottom', 'top']} background="dark">
            <GridBlock
              contents={[
                {
                  content:
		  'Have you ever choked on the QNetworkAccessManager to download large files and also resume downloads '+
	  	  'automatically ? I did , So thats why I created this small library which made my day easier and so I '+
		  'think it will make yours too. This small library helps you to download files that you queue and also '+
		  'automatically resumes the downloads (Partial Download ?). QEasyDownloader also '+
		  'handles network interruptions automatically! And the best part is ,it comes in a single source and header file!',
                  title: 'Why Create QEasyDownloader ?',
                },
              ]}
            />
          </Container>

          <div className="productShowcaseSection paddingBottom">
            <h2>{"Who's Using This?"}</h2>
            <p>This project is used by all these people</p>
            <div className="logos">{showcase}</div>
            <div className="more-users">
              <a
                className="button"
                href={
                  siteConfig.baseUrl + this.props.language + '/' + 'users.html'
                }>
                More {siteConfig.title} Users
              </a>
            </div>
          </div>
        </div>
      </div>
    );
  }
}

module.exports = Index;
