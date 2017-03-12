#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <initializer_list>
#include "../config/Config.h"
#include "../sprites.h"
#include "Award.h"

extern "C"
{
    #include "../interface/window.h"
    #include "../localisation/localisation.h"
    #include "../peep/peep.h"
    #include "../ride/ride.h"
    #include "../scenario/scenario.h"
    #include "../world/sprite.h"
    #include "news_item.h"
}

Award gCurrentAwards[MAX_AWARDS];

namespace Awards
{
    #pragma region Award checks

    /** More than 1/16 of the total guests must be thinking untidy thoughts. */
    static bool award_is_deserved_most_untidy(sint32 awardType, sint32 activeAwardTypes)
    {
        if (activeAwardTypes & (1 << PARK_AWARD_MOST_BEAUTIFUL)) return 0;
        if (activeAwardTypes & (1 << PARK_AWARD_BEST_STAFF)) return 0;
        if (activeAwardTypes & (1 << PARK_AWARD_MOST_TIDY)) return 0;

        sint32 negativeCount = 0;

        uint16 spriteIndex;
        rct_peep * peep;
        FOR_ALL_GUESTS(spriteIndex, peep)
        {
            if (peep->outside_of_park == 0)
            {
                const rct_peep_thought * thought = &peep->thoughts[0];
                if (thought->var_2 <= 5)
                {
                    if (thought->type == PEEP_THOUGHT_TYPE_BAD_LITTER ||
                        thought->type == PEEP_THOUGHT_TYPE_PATH_DISGUSTING ||
                        thought->type == PEEP_THOUGHT_TYPE_VANDALISM)
                    {
                        negativeCount++;
                    }
                }
            }
        }

        return (negativeCount > gNumGuestsInPark / 16);
    }

    /** More than 1/64 of the total guests must be thinking tidy thoughts and less than 6 guests thinking untidy thoughts. */
    static bool award_is_deserved_most_tidy(sint32 awardType, sint32 activeAwardTypes)
    {
        if (activeAwardTypes & (1 << PARK_AWARD_MOST_UNTIDY)) return 0;
        if (activeAwardTypes & (1 << PARK_AWARD_MOST_DISAPPOINTING)) return 0;

        sint32 positiveCount = 0;
        sint32 negativeCount = 0;

        uint16 spriteIndex;
        rct_peep * peep;
        FOR_ALL_GUESTS(spriteIndex, peep)
        {
            if (peep->outside_of_park == 0)
            {
                const rct_peep_thought * thought = &peep->thoughts[0];
                if (thought->var_2 <= 5)
                {
                    if (thought->type == PEEP_THOUGHT_TYPE_VERY_CLEAN)
                    {
                        positiveCount++;
                    }
                    if (thought->type == PEEP_THOUGHT_TYPE_BAD_LITTER ||
                        thought->type == PEEP_THOUGHT_TYPE_PATH_DISGUSTING ||
                        thought->type == PEEP_THOUGHT_TYPE_VANDALISM)
                    {
                        negativeCount++;
                    }
                }
            }
        }

        return (negativeCount <= 5 && positiveCount > gNumGuestsInPark / 64);
    }

    /** At least 6 open roller coasters. */
    static bool award_is_deserved_best_rollercoasters(sint32 awardType, sint32 activeAwardTypes)
    {
        sint32 rollerCoasters = 0;

        sint32 i;
        rct_ride * ride;
        FOR_ALL_RIDES(i, ride)
        {
            if (ride_is_running(ride))
            {
                rct_ride_entry * rideEntry = get_ride_entry(ride->subtype);
                if (rideEntry != nullptr && ride_entry_is_in_category(rideEntry, RIDE_GROUP_ROLLERCOASTER))
                {
                    rollerCoasters++;
                }
            }
        }

        return (rollerCoasters >= 6);
    }

    /** Entrance fee is 0.10 less than half of the total ride value. */
    static bool award_is_deserved_best_value(sint32 awardType, sint32 activeAwardTypes)
    {
        if (activeAwardTypes & (1 << PARK_AWARD_WORST_VALUE)) return 0;
        if (activeAwardTypes & (1 << PARK_AWARD_MOST_DISAPPOINTING)) return 0;

        return (gTotalRideValue < MONEY(10, 00) ||
                park_get_entrance_fee() + MONEY(0, 10) >= gTotalRideValue / 2);
    }

    /** More than 1/128 of the total guests must be thinking scenic thoughts and less than 16 untidy thoughts. */
    static bool award_is_deserved_most_beautiful(sint32 awardType, sint32 activeAwardTypes)
    {
        if (activeAwardTypes & (1 << PARK_AWARD_MOST_UNTIDY)) return 0;
        if (activeAwardTypes & (1 << PARK_AWARD_MOST_DISAPPOINTING)) return 0;

        sint32 positiveCount = 0;
        sint32 negativeCount = 0;
        uint16 spriteIndex;
        rct_peep * peep;
        FOR_ALL_GUESTS(spriteIndex, peep)
        {
            if (peep->outside_of_park == 0)
            {
                const rct_peep_thought * thought = &peep->thoughts[0];
                if (thought->var_2 <= 5)
                {
                    if (thought->type == PEEP_THOUGHT_TYPE_SCENERY)
                    {
                        positiveCount++;
                    }
                    if (thought->type == PEEP_THOUGHT_TYPE_BAD_LITTER ||
                        thought->type == PEEP_THOUGHT_TYPE_PATH_DISGUSTING ||
                        thought->type == PEEP_THOUGHT_TYPE_VANDALISM)
                    {
                        negativeCount++;
                    }
                }
            }
        }

        return (negativeCount <= 15 && positiveCount > gNumGuestsInPark / 128);
    }

    /** Entrance fee is more than total ride value. */
    static bool award_is_deserved_worse_value(sint32 awardType, sint32 activeAwardTypes)
    {
        if (activeAwardTypes & (1 << PARK_AWARD_BEST_VALUE))
        {
            return 0;
        }

        money32 parkEntranceFee = park_get_entrance_fee();
        return (parkEntranceFee == MONEY_FREE || gTotalRideValue >= parkEntranceFee);
    }

    /** No more than 2 people who think the vandalism is bad and no crashes. */
    static bool award_is_deserved_safest(sint32 awardType, sint32 activeAwardTypes)
    {
        sint32 peepsWhoDislikeVandalism = 0;
        uint16 spriteIndex;
        rct_peep * peep;
        FOR_ALL_GUESTS(spriteIndex, peep)
        {
            if (peep->outside_of_park == 0 && peep_is_thinking(peep, PEEP_THOUGHT_TYPE_VANDALISM))
            {
                peepsWhoDislikeVandalism++;
            }
        }

        if (peepsWhoDislikeVandalism > 2)
        {
            return 0;
        }

        // Check for rides that have crashed recently
        sint32 i;
        rct_ride * ride;
        FOR_ALL_RIDES(i, ride)
        {
            if (ride->last_crash_type != RIDE_CRASH_TYPE_NONE)
            {
                return 0;
            }
        }

        return 1;
    }

    /** All staff types, at least 20 staff, one staff per 32 peeps. */
    static bool award_is_deserved_best_staff(sint32 awardType, sint32 activeAwardTypes)
    {
        if (activeAwardTypes & (1 << PARK_AWARD_MOST_UNTIDY))
        {
            return 0;
        }

        sint32 peepCount = 0;
        sint32 staffCount = 0;
        sint32 staffTypeFlags = 0;
        uint16 spriteIndex;
        rct_peep * peep;
        FOR_ALL_PEEPS(spriteIndex, peep)
        {
            if (peep->type == PEEP_TYPE_STAFF)
            {
                staffCount++;
                staffTypeFlags |= (1 << peep->staff_type);
            }
            else
            {
                peepCount++;
            }
        }

        return ((staffTypeFlags & 0xF) && staffCount >= 20 && staffCount >= peepCount / 32);
    }

    /** At least 7 shops, 4 unique, one shop per 128 guests and no more than 12 hungry guests. */
    static bool award_is_deserved_best_food(sint32 awardType, sint32 activeAwardTypes)
    {
        if (activeAwardTypes & (1 << PARK_AWARD_WORST_FOOD))
        {
            return 0;
        }

        sint32 shops = 0;
        sint32 uniqueShops = 0;
        sint32 shopTypes = 0;
        sint32 i;
        rct_ride * ride;
        FOR_ALL_RIDES(i, ride)
        {
            if (ride->status == RIDE_STATUS_OPEN && ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_SELLS_FOOD))
            {
                shops++;

                rct_ride_entry * rideEntry = get_ride_entry(ride->subtype);
                if (rideEntry != nullptr)
                {
                    if (!(shopTypes & (1ULL << rideEntry->shop_item)))
                    {
                        shopTypes |= (1ULL << rideEntry->shop_item);
                        uniqueShops++;
                    }
                }
            }
        }

        if (shops < 7 || uniqueShops < 4 || shops < gNumGuestsInPark / 128)
        {
            return 0;
        }

        sint32 hungryPeeps = get_hungry_guests_count();
        return (hungryPeeps <= 12);
    }

    /** No more than 2 unique shops, less than one shop per 256 guests and more than 15 hungry guests. */
    static bool award_is_deserved_worst_food(sint32 awardType, sint32 activeAwardTypes)
    {
        if (activeAwardTypes & (1 << PARK_AWARD_BEST_FOOD))
        {
            return 0;
        }

        sint32 shops = 0;
        sint32 uniqueShops = 0;
        sint32 shopTypes = 0;
        sint32 i;
        rct_ride * ride;
        FOR_ALL_RIDES(i, ride)
        {
            if (ride->status == RIDE_STATUS_OPEN && ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_SELLS_FOOD))
            {
                shops++;

                rct_ride_entry * rideEntry = get_ride_entry(ride->subtype);
                if (rideEntry != nullptr)
                {
                    if (!(shopTypes & (1ULL << rideEntry->shop_item)))
                    {
                        shopTypes |= (1ULL << rideEntry->shop_item);
                        uniqueShops++;
                    }
                }
            }
        }

        if (uniqueShops > 2 || shops > gNumGuestsInPark / 256)
        {
            return 0;
        }

        sint32 hungryGuests = get_hungry_guests_count();
        return (hungryGuests > 15);
    }

    /** At least 4 restrooms, 1 restroom per 128 guests and no more than 16 guests who think they need the restroom. */
    static bool award_is_deserved_best_restrooms(sint32 awardType, sint32 activeAwardTypes)
    {
        uint32 numToilets = get_open_toilets_count();
        if (numToilets < 4)
        {
            return 0;
        }

        // At least one open restroom for every 128 guests
        if (numToilets < gNumGuestsInPark / 128U)
        {
            return 0;
        }

        // Count number of guests who are thinking they need the restroom
        uint32 guestsWhoNeedToilet = 0;
        uint16 spriteIndex;
        rct_peep * peep;
        FOR_ALL_GUESTS(spriteIndex, peep)
        {
            if (peep->outside_of_park == 0 && peep_is_thinking(peep, PEEP_THOUGHT_TYPE_BATHROOM))
            {
                guestsWhoNeedToilet++;
            }
        }
        return (guestsWhoNeedToilet <= 16);
    }

    /** More than half of the rides have satisfaction <= 6 and park rating <= 650. */
    static bool award_is_deserved_most_disappointing(sint32 awardType, sint32 activeAwardTypes)
    {
        if (activeAwardTypes & (1 << PARK_AWARD_BEST_VALUE))
        {
            return 0;
        }

        if (gParkRating > 650)
        {
            return 0;
        }

        // Count the number of disappointing rides
        uint32 countedRides = 0;
        uint32 disappointingRides = 0;

        uint32 i;
        rct_ride * ride;
        FOR_ALL_RIDES(i, ride)
        {
            if (ride_has_ratings(ride) && ride->popularity != 0xFF)
            {
                countedRides++;
                if (ride->popularity <= 6)
                {
                    disappointingRides++;
                }
            }
        }

        // Half of the rides are disappointing
        return (disappointingRides >= countedRides / 2);
    }

    /** At least 6 open water rides. */
    static bool award_is_deserved_best_water_rides(sint32 awardType, sint32 activeAwardTypes)
    {
        sint32 waterRides = 0;

        sint32 i;
        rct_ride * ride;
        FOR_ALL_RIDES(i, ride)
        {
            if (ride_is_running(ride))
            {
                rct_ride_entry * rideEntry = get_ride_entry(ride->subtype);
                if (rideEntry != nullptr && ride_entry_is_in_category(rideEntry, RIDE_GROUP_WATER))
                {
                    waterRides++;
                }
            }
        }

        return (waterRides >= 6);
    }

    /** At least 6 custom designed rides. */
    static bool award_is_deserved_best_custom_designed_rides(sint32 awardType, sint32 activeAwardTypes)
    {
        if (activeAwardTypes & (1 << PARK_AWARD_MOST_DISAPPOINTING))
        {
            return 0;
        }

        sint32 customDesignedRides = 0;
        sint32 i;
        rct_ride * ride;
        FOR_ALL_RIDES(i, ride)
        {
            if (ride->status == RIDE_STATUS_OPEN &&
                !(ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED) &&
                !(ride->lifecycle_flags & RIDE_LIFECYCLE_NOT_CUSTOM_DESIGN) &&
                ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK) &&
                ride->excitement >= RIDE_RATING(5, 50))
            {
                customDesignedRides++;
            }
        }

        return (customDesignedRides >= 6);
    }

    /** At least 5 colourful rides and more than half of the rides are colourful. */
    static bool award_is_deserved_most_dazzling_ride_colours(sint32 awardType, sint32 activeAwardTypes)
    {
        static const std::initializer_list<colour_t> DazzlingRideColours =
        {
            COLOUR_BRIGHT_PURPLE,
            COLOUR_BRIGHT_GREEN,
            COLOUR_LIGHT_ORANGE,
            COLOUR_BRIGHT_PINK,
        };

        if (activeAwardTypes & (1 << PARK_AWARD_MOST_DISAPPOINTING))
        {
            return 0;
        }

        sint32 countedRides = 0;
        sint32 colourfulRides = 0;
        sint32 i;
        rct_ride * ride;
        FOR_ALL_RIDES(i, ride)
        {
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK))
            {
                countedRides++;

                uint8 mainTrackColour = ride->track_colour_main[0];
                for (colour_t colour : DazzlingRideColours)
                {
                    if (mainTrackColour == colour)
                    {
                        colourfulRides++;
                        break;
                    }
                }
            }
        }

        return (colourfulRides >= 5 && colourfulRides >= countedRides - colourfulRides);
    }

    /** At least 10 peeps and more than 1/64 of total guests are lost or can't find something. */
    static bool award_is_deserved_most_confusing_layout(sint32 awardType, sint32 activeAwardTypes)
    {
        uint32 peepsCounted = 0;
        uint32 peepsLost = 0;
        uint16 spriteIndex;
        rct_peep * peep;
        FOR_ALL_GUESTS(spriteIndex, peep)
        {
            if (peep->outside_of_park == 0)
            {
                peepsCounted++;

                if (peep_is_thinking(peep, PEEP_THOUGHT_TYPE_LOST) ||
                    peep_is_thinking(peep, PEEP_THOUGHT_TYPE_CANT_FIND))
                {
                    peepsLost++;
                }
            }
        }

        return (peepsLost >= 10 && peepsLost >= peepsCounted / 64);
    }

    /** At least 10 open gentle rides. */
    static bool award_is_deserved_best_gentle_rides(sint32 awardType, sint32 activeAwardTypes)
    {
        sint32 gentleRides = 0;
        sint32 i;
        rct_ride * ride;
        FOR_ALL_RIDES(i, ride)
        {
            if (ride_is_running(ride))
            {
                rct_ride_entry * rideEntry = get_ride_entry(ride->subtype);
                if (rideEntry != nullptr && ride_entry_is_in_category(rideEntry, RIDE_GROUP_GENTLE))
                {
                    gentleRides++;
                }
            }
        }
        return (gentleRides >= 10);
    }

    constexpr bool NEGATIVE = true;
    constexpr bool POSITIVE = false;

    static const AwardDescriptor AwardDescriptors[] =
    {
        { award_is_deserved_most_untidy,                SPR_AWARD_MOST_UNTIDY,                  STR_AWARD_MOST_UNTIDY,                  STR_NEWS_ITEM_AWARD_MOST_UNTIDY,            NEGATIVE },
        { award_is_deserved_most_tidy,                  SPR_AWARD_MOST_TIDY,                    STR_AWARD_MOST_TIDY,                    STR_NEWS_ITEM_MOST_TIDY,                    POSITIVE },
        { award_is_deserved_best_rollercoasters,        SPR_AWARD_BEST_ROLLERCOASTERS,          STR_AWARD_BEST_ROLLERCOASTERS,          STR_NEWS_ITEM_BEST_ROLLERCOASTERS,          POSITIVE },
        { award_is_deserved_best_value,                 SPR_AWARD_BEST_VALUE,                   STR_AWARD_BEST_VALUE,                   STR_NEWS_ITEM_BEST_VALUE,                   POSITIVE },
        { award_is_deserved_most_beautiful,             SPR_AWARD_MOST_BEAUTIFUL,               STR_AWARD_MOST_BEAUTIFUL,               STR_NEWS_ITEM_MOST_BEAUTIFUL,               POSITIVE },
        { award_is_deserved_worse_value,                SPR_AWARD_WORST_VALUE,                  STR_AWARD_WORST_VALUE,                  STR_NEWS_ITEM_WORST_VALUE,                  NEGATIVE },
        { award_is_deserved_safest,                     SPR_AWARD_SAFEST,                       STR_AWARD_SAFEST,                       STR_NEWS_ITEM_SAFEST,                       POSITIVE },
        { award_is_deserved_best_staff,                 SPR_AWARD_BEST_STAFF,                   STR_AWARD_BEST_STAFF,                   STR_NEWS_ITEM_BEST_STAFF,                   POSITIVE },
        { award_is_deserved_best_food,                  SPR_AWARD_BEST_FOOD,                    STR_AWARD_BEST_FOOD,                    STR_NEWS_ITEM_BEST_FOOD,                    POSITIVE },
        { award_is_deserved_worst_food,                 SPR_AWARD_WORST_FOOD,                   STR_AWARD_WORST_FOOD,                   STR_NEWS_ITEM_WORST_FOOD,                   NEGATIVE },
        { award_is_deserved_best_restrooms,             SPR_AWARD_BEST_RESTROOMS,               STR_AWARD_BEST_RESTROOMS,               STR_NEWS_ITEM_BEST_RESTROOMS,               POSITIVE },
        { award_is_deserved_most_disappointing,         SPR_AWARD_MOST_DISAPPOINTING,           STR_AWARD_MOST_DISAPPOINTING,           STR_NEWS_ITEM_MOST_DISAPPOINTING,           NEGATIVE },
        { award_is_deserved_best_water_rides,           SPR_AWARD_BEST_WATER_RIDES,             STR_AWARD_BEST_WATER_RIDES,             STR_NEWS_ITEM_BEST_WATER_RIDES,             POSITIVE },
        { award_is_deserved_best_custom_designed_rides, SPR_AWARD_BEST_CUSTOM_DESIGNED_RIDES,   STR_AWARD_BEST_CUSTOM_DESIGNED_RIDES,   STR_NEWS_ITEM_BEST_CUSTOM_DESIGNED_RIDES,   POSITIVE },
        { award_is_deserved_most_dazzling_ride_colours, SPR_AWARD_MOST_DAZZLING_RIDE_COLOURS,   STR_AWARD_MOST_DAZZLING_RIDE_COLOURS,   STR_NEWS_ITEM_MOST_DAZZLING_RIDE_COLOURS,   POSITIVE },
        { award_is_deserved_most_confusing_layout,      SPR_AWARD_MOST_CONFUSING_LAYOUT,        STR_AWARD_MOST_CONFUSING_LAYOUT,        STR_NEWS_ITEM_MOST_CONFUSING_LAYOUT,        NEGATIVE },
        { award_is_deserved_best_gentle_rides,          SPR_AWARD_BEST_GENTLE_RIDES,            STR_AWARD_BEST_GENTLE_RIDES,            STR_NEWS_ITEM_BEST_GENTLE_RIDES,            POSITIVE },
    };

    static bool IsDeserved(sint32 awardType, sint32 activeAwardTypes)
    {
        auto awardDesc = &AwardDescriptors[awardType];
        auto checkFunc = awardDesc->CheckFunc;
        return checkFunc(awardType, activeAwardTypes);
    }

    #pragma endregion

    static sint32 GetSpareAwardSlotIndex()
    {
        for (sint32 i = 0; i < MAX_AWARDS; i++)
        {
            if (gCurrentAwards[i].Time == 0)
            {
                return i;
            }
        }
        return -1;
    }

    static uint32 GetActiveAwardTypes()
    {
        uint32 activeAwardTypes = 0;
        for (sint32 i = 0; i < MAX_AWARDS; i++)
        {
            Award * award = &gCurrentAwards[i];
            if (award->Time != 0)
            {
                activeAwardTypes |= (1 << award->Type);
            }
        }
        return activeAwardTypes;
    }

    static PARK_AWARD GetRandomAwardToAchieve(uint32 activeAwardTypes)
    {
        PARK_AWARD result = PARK_AWARD_UNDEFINED;
        PARK_AWARD potentialAwards[PARK_AWARD_COUNT];
        sint32 potentialAwardsCount = 0;

        for (uint32 i = 0; i < PARK_AWARD_COUNT; i++)
        {
            if (!(activeAwardTypes & (1 << i)))
            {
                potentialAwards[potentialAwardsCount] = (PARK_AWARD)i;
                potentialAwardsCount++;
            }
        }

        if (potentialAwardsCount > 0)
        {
            uint32 randomIndex = scenario_rand() % potentialAwardsCount;
            result = potentialAwards[randomIndex];
        }
        return result;
    }

    static void AchieveNewAward(PARK_AWARD type)
    {
        sint32 index = GetSpareAwardSlotIndex();
        if (index != -1)
        {
            gCurrentAwards[index].Type = type;
            gCurrentAwards[index].Time = 5;
            if (gConfigNotifications.park_award)
            {
                news_item_add_to_queue(NEWS_ITEM_AWARD, AwardDescriptors[type].NewsMessageText, 0);
            }
            window_invalidate_by_class(WC_PARK_INFORMATION);
        }
    }

    static void DecrementAwardTimes()
    {
        for (sint32 i = 0; i < MAX_AWARDS; i++)
        {
            Award * award = &gCurrentAwards[i];
            if (award->Time != 0)
            {
                award->Time--;
                if (award->Time == 0)
                {
                    window_invalidate_by_class(WC_PARK_INFORMATION);
                }
            }
        }
    }

    static void Update()
    {
        if (park_is_open())
        {
            uint32 activeAwardTypes = GetActiveAwardTypes();
            PARK_AWARD awardType = GetRandomAwardToAchieve(activeAwardTypes);
            if (awardType != PARK_AWARD_UNDEFINED)
            {
                if (IsDeserved(awardType, activeAwardTypes))
                {
                    AchieveNewAward((PARK_AWARD)awardType);
                }
            }
        }
        DecrementAwardTimes();
    }
}

extern "C"
{
    void award_reset()
    {
        for (sint32 i = 0; i < MAX_AWARDS; i++)
        {
            gCurrentAwards[i].Time = 0;
            gCurrentAwards[i].Type = 0;
        }
    }

    void award_update_all()
    {
        Awards::Update();
    }

    const AwardDescriptor * award_get_descriptor(sint32 type)
    {
        return &Awards::AwardDescriptors[type];
    }
}
