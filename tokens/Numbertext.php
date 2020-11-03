<?php

require_once('Soros.php');

/**
 * Main classes used by the Numbertext extension.
 * Is based on the source code from http://numbertext.org/
 *
 * @author Pavel Astakhov <pastakhov@yandex.ru>
 * @author Sebastian Lisken <sebastian.lisken@civiservice.de>
 * @license LGPL/BSD dual-license
 */
class Numbertext {

	private static $numbertextLang = [
		'af' => 'af_ZA',
		'ca' => 'ca_ES',
		'cs' => 'cs_CZ',
		'da' => 'da_DK',
		'de' => 'de_DE',
		'el' => 'el_EL',
		'en' => 'en_US',
		'es' => 'es_ES',
		'fi' => 'fi_FI',
		'fr' => 'fr_FR',
		'he' => 'he_IL',
		'hu' => 'hu_HU',
		'id' => 'id_ID',
		'it' => 'it_IT',
		'ja' => 'ja_JP',
		'ko' => 'ko_KR',
		'lb' => 'lb_LU',
		'lt' => 'lt_LT',
		'lv' => 'lv_LV',
		'nl' => 'nl_NL',
		'pl' => 'pl_PL',
		'pt' => 'pt_PT',
		'ro' => 'ro_RO',
		'ru' => 'ru_RU',
		'sh' => 'sh_RS',
		'sl' => 'sl_SI',
		'sr' => 'sr_RS',
		'sv' => 'sv_SE',
		'th' => 'th_TH',
		'tr' => 'tr_TR',
		'vi' => 'vi_VN',
		'zh' => 'zh_ZH',
	];

	private static function loadFile($lang) {
		$url = __DIR__ . "/data/" . $lang . ".sor";
		$soros_text = @file_get_contents($url);
		if ($soros_text === false) {
			return null;
		}
		$soros = new Soros($soros_text);
		return $soros;
	}

	private static $modules = [];

	private static function addModule($lang, $soros) {
		self::$modules[$lang] = $soros;
	}

	private static function loadFor($lang) {
		$_lang = strtr($lang, "-", "_");
		$soros = self::loadFile($_lang);
		if ($soros !== null) {
			self::addModule($lang, $soros);
			return $soros;
		}
		if (strpos($_lang, "_") !== false) {
			$_lang = preg_replace("/_.*/", "", $_lang);
			$soros = self::loadFile($_lang);
			if ($soros !== null) {
				self::addModule($lang, $soros);
				return $soros;
			}
		}
		$_lang = @self::$numbertextLang[$_lang];
		if (@strlen($_lang) > 0) {
			$soros = self::loadFile($_lang);
			if ($soros !== null) {
				self::addModule($lang, $soros);
				return $soros;
			}
		}
		return null;
	}

	private static function getLangModule($lang) {
		return self::$modules[$lang] ?? self::loadFor($lang) ?? self::loadFor("en_US");
	}

	public function __construct() {
	}

	/**
	 * Number to text conversion
	 *
	 * @param string $input
	 * @param string $lang default 'en_US'
	 * @return string
	 */
	public static function numbertext($input = '', $lang = '') {
		$soros = self::getLangModule($lang);
		return $soros === null ? null : $soros->run($input);
	}

	/**
	 * Money to text conversion
	 *
	 * @param string $input
	 * @param string $money
	 * @param string $lang default 'en_US'
	 * @return string
	 */
	public static function moneytext($input = '', $money = '', $lang = '') {
		return self::numbertext($money . " " . $input, $lang);
	}

}

