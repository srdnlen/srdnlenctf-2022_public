<?php

// autoload_static.php @generated by Composer

namespace Composer\Autoload;

class ComposerStaticInit68c5416a6a04af578140daab9c1d1862
{
    public static $prefixLengthsPsr4 = array (
        'F' => 
        array (
            'Firebase\\JWT\\' => 13,
        ),
    );

    public static $prefixDirsPsr4 = array (
        'Firebase\\JWT\\' => 
        array (
            0 => __DIR__ . '/..' . '/firebase/php-jwt/src',
        ),
    );

    public static $classMap = array (
        'Composer\\InstalledVersions' => __DIR__ . '/..' . '/composer/InstalledVersions.php',
    );

    public static function getInitializer(ClassLoader $loader)
    {
        return \Closure::bind(function () use ($loader) {
            $loader->prefixLengthsPsr4 = ComposerStaticInit68c5416a6a04af578140daab9c1d1862::$prefixLengthsPsr4;
            $loader->prefixDirsPsr4 = ComposerStaticInit68c5416a6a04af578140daab9c1d1862::$prefixDirsPsr4;
            $loader->classMap = ComposerStaticInit68c5416a6a04af578140daab9c1d1862::$classMap;

        }, null, ClassLoader::class);
    }
}
